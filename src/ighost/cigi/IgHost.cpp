
#include "mixr/ighost/cigi/IgHost.hpp"

#include "mixr/ighost/cigi/TypeMapper.hpp"
#include "mixr/ighost/cigi/CigiModel.hpp"

#include "mixr/models/player/Player.hpp"

#include "mixr/models/player/weapon/AbstractWeapon.hpp"

#include "mixr/simulation/AbstractNib.hpp"

#include "mixr/base/Identifier.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/PairStream.hpp"
#include "mixr/base/numeric/Number.hpp"
#include "mixr/base/osg/Vec3d"
#include "mixr/base/units/Distances.hpp"

#include <cstring>
#include <cmath>

namespace mixr {
namespace cigi {

IMPLEMENT_ABSTRACT_SUBCLASS(IgHost, "BaseIgHost")

BEGIN_SLOTTABLE(IgHost)
   "maxRange",         // 1: Max range of visual system (distance: meters)
   "maxModels",        // 2: Max number of models
   "maxElevations",    // 3: Max number of terrain elevation requests
   "latitude",         // 4: Visual reference latitude (deg)
   "longitude",        // 5: Visual reference longitude (deg)
   "typeMap",          // 6: IG's system model type IDs (PairStream of TypeMapper objects)
END_SLOTTABLE(IgHost)

BEGIN_SLOT_MAP(IgHost)
   ON_SLOT(1, setSlotMaxRange,      base::Distance)
   ON_SLOT(1, setSlotMaxRange,      base::Number)
   ON_SLOT(2, setSlotMaxModels,     base::Number)
   ON_SLOT(3, setSlotMaxElevations, base::Number)
   ON_SLOT(4, setSlotRefLatitude,   base::Number)
   ON_SLOT(5, setSlotRefLongitude,  base::Number)
   ON_SLOT(6, setSlotTypeMap,       base::PairStream)
END_SLOT_MAP()

IgHost::IgHost()
{
   STANDARD_CONSTRUCTOR()
}

void IgHost::copyData(const IgHost& org, const bool)
{
   BaseClass::copyData(org);

   resetTables();

   clearModelTypes();
   for (int i = 0; i < org.nOtwModelTypes; i++) {
      org.otwModelTypes[i]->ref();
      otwModelTypes[i] = org.otwModelTypes[i];
      nOtwModelTypes++;
   }

   maxRange = org.maxRange;
   maxModels = org.maxModels;
   maxElevations = org.maxElevations;
   rstFlg = org.rstFlg;
   rstReq = org.rstReq;

   // Posiiton
   refLat = org.refLat;
   refLon = org.refLon;

   setOwnship(org.ownship);
   setPlayerList(org.playerList);
}

void IgHost::deleteData()
{
   setOwnship(nullptr);
   setPlayerList(nullptr);
   resetTables();
   clearModelTypes();
}

//------------------------------------------------------------------------------
// reset() -- Reset the visual system interface
//------------------------------------------------------------------------------
void IgHost::reset()
{
   BaseClass::reset();
   setPlayerList(nullptr);
   rstReq = true;
}

//------------------------------------------------------------------------------
// resetTables() -- Resets all of the working tables
//------------------------------------------------------------------------------
void IgHost::resetTables()
{
   // Clear the model table
   // (in reverse order just in case another thread is traversing the
   //  table from bottom up)
   while (nModels > 0) {
      removeModelFromList(nModels-1, MODEL_TABLE);
   }

   // Clear the elevation table
   // (in reverse order just in case another thread is traversing the
   //  table from bottom up)
   while (nHots > 0) {
      removeModelFromList(nHots-1, HOT_TABLE);
   }
}

//------------------------------------------------------------------------------
// clearOtwModelTypes() -- Clear the OTW model types table
//------------------------------------------------------------------------------
void IgHost::clearModelTypes()
{
   // Clear our old OTW model type table --
   // Just in case someone is scanning the table, we clear the entries
   // and decrement 'nOtwModelTypes' before the object is unref()'ed
   while (nOtwModelTypes > 0) {
      nOtwModelTypes--;
      otwModelTypes[nOtwModelTypes]->unref();
      otwModelTypes[nOtwModelTypes] = nullptr;
   }
}

void IgHost::updateIg(const double)
{
   // Check reset flag
   if (rstReq) {
      rstFlg = true;
      rstReq = false;
   }

   // update one visual system frame
   processesModels();
   processesElevations();
   frameSync();

   rstFlg = false;
}

//------------------------------------------------------------------------------
// processesModels() -- Process ownship & player models
//------------------------------------------------------------------------------
void IgHost::processesModels()
{
   mapPlayerList2ModelTable();     // Map current player list to model table
   sendOwnshipAndModels();         // Send ownship model & model table
}

//------------------------------------------------------------------------------
// processesElevations() -- Process terrain elevation requests
//------------------------------------------------------------------------------
void IgHost::processesElevations()
{
   recvElevations();               // Received previous elevation requests
   mapPlayers2ElevTable();         // Map new elevation requests
   sendElevationRequests();        // and send the new requests
}

//------------------------------------------------------------------------------
// mapPlayerList2ModelTable() - Map the player list to the model table
//
//  model states are:
//     INACTIVE     -- unused model entry
//     ACTIVE       -- player is alive and in-range
//     DEAD         -- player is dead or destroyed
//     OUT_OF_RANGE -- player is alive but out of range
//
//  Note: this routines will set model entries to DEAD and OUT_OF_RANGE, but the
//  derived class should handle the visual system unique termination sequences and
//  clear the model entry.
//------------------------------------------------------------------------------
void IgHost::mapPlayerList2ModelTable()
{
   // ---
   // Check for reset
   // ---
   if (isResetInProgress()) {
      // Set all active models as Out-Of-Range so that sendOwnshipAndModels() can remove them
      for (int i = 0; i < getModelTableSize(); i++) {
         modelTbl[i]->setState( CigiModel::OUT_OF_RANGE );
      }
      return;
   }

   // ---
   // Remove all inactive, dead or out-of-range models
   //   -- These states were issued last pass, so the OTW system
   //       specific software should have handled them by now.
   //   -- As models are removed, the table above the model is shifted down.
   //   -- We're also clearing the model's 'checked' flag
   // ---
   for (int i = getModelTableSize(); i > 0; --i) {
      if ( modelTbl[i-1]->isState(CigiModel::CLEARED) ) {
         // Deleting this model
         //std::cout << "Otw::mapPlayerList2ModelTable() cleanup: model = " << modelTbl[i] << std::endl;
         removeModelFromList( (i-1), MODEL_TABLE);
      }
   }
   for (int i = 0; i < getModelTableSize(); i++) {
      modelTbl[i]->setCheckedFlag(false);
   }

   if (playerList != nullptr) {
      // We must have a player list ...

      // ---
      // Find players that are alive and within range of the visual system ...
      // ---
      base::List::Item* item{playerList->getFirstItem()};
      while (item != nullptr) {

         // Get a pointer to the player, 'p'
         const auto pair = static_cast<base::Pair*>(item->getValue());
         const auto p = static_cast<models::Player*>(pair->object());

         bool dummy{};
         const auto wpn = dynamic_cast<const models::AbstractWeapon*>( p );
         if (wpn != nullptr) dummy = wpn->isDummy();

         if ( p != getOwnship() && !dummy ) {

            // Find the player's model entry (if any)
            CigiModel* model = findModel(p, MODEL_TABLE);

            // Check if in-range
            bool inRange {computeRangeToPlayer(p) <= maxRange};

            // Check if this player is alive and within range.
            if (p->isActive() && inRange) {
               // When alive and in range ...
               if (model != nullptr) {
                  // a) and it already has a model entry: make sure it's active ...
                  model->setState( CigiModel::ACTIVE );
               } else {
                  // b) and it doesn't have a model entry (new, in-range player) ...
                  model = newModelEntry(p);
               }
            } else if (p->isDead() && inRange) {
               // When player isn't alive and it had a model entry
               if (model != nullptr) {
                  // set state to dead
                  model->setState( CigiModel::DEAD );
               }
            } else {
               // When player is out-of-range and it had a model entry
               if (model != nullptr) {
                  // set state to out-of-range
                  model->setState( CigiModel::OUT_OF_RANGE );
               }
            }
            if (model != nullptr) model->setCheckedFlag(true);

         }

         item = item->getNext(); // Next player
      }

   }

   // ---
   // Any models not checked needs to be removed
   // ---
   for (int i = 0; i < getModelTableSize(); i++) {
      if ( modelTbl[i]->isNotChecked() ) {
         // Request removal;
         // (note: the OTW system specific code now has one frame to cleanup its own code
         //  before the model is dropped from the output list next frame -- see above)
         modelTbl[i]->setState( CigiModel::OUT_OF_RANGE );
      }
   }

}

//------------------------------------------------------------------------------
// mapPlayers2ElevTable() - Map the player list to the model table
//------------------------------------------------------------------------------
void IgHost::mapPlayers2ElevTable()
{
   // Set all entries as unchecked
   for (int i = 0; i < getElevationTableSize(); i++) {
      hotTbl[i]->setCheckedFlag(false);
   }

   // When we have a player list ...
   if (playerList != nullptr) {

      // ---
      // Find players that are alive and require terrain elevation from the visual system ...
      // ---
      base::List::Item* item{playerList->getFirstItem()};
      while (item != nullptr) {

         // Get a pointer to the player, 'p'
         base::Pair* pair {static_cast<base::Pair*>(item->getValue())};
         models::Player* p {static_cast<models::Player*>(pair->object())};

         // Check if this player is alive and within range.
         if ( p->isActive() && p->isTerrainElevationRequired() ) {

            // Check if in-range
            const bool inRange {computeRangeToPlayer(p) <= maxRange};

            if (inRange) {

               // Find the player's model entry (if any)
               CigiModel* model {findModel(p, HOT_TABLE)};

               if (model != nullptr) {
                  // The player has a valid entry.
                  model->incReqCount();
               } else {
                  // Player doesn't have an entry, so create one.
                  model = newElevEntry(p);
               }
               if (model != nullptr) model->setCheckedFlag(true);
            }

         }

         //completed  = p->isNetworkedPlayer();
         item = item->getNext(); // Next player
      }
   }

   // ---
   // Remove unmatched model entries; their players are inactive or no longer
   // require terrain elevation
   // ---
   for (int i = getElevationTableSize(); i > 0; --i) {
      if ( hotTbl[i-1]->isNotChecked() ) {
         // Deleting this entry
         removeModelFromList( (i-1), HOT_TABLE);
      }
   }
}

//------------------------------------------------------------------------------
// computeRangeToPlayer() -- Calculate range from ownship to player
//------------------------------------------------------------------------------
double IgHost::computeRangeToPlayer(const models::Player* const ip) const
{
    double rng{maxRange*2.0 + 1.0};  // Default is out-of-range
    if (ownship != nullptr) {
        base::Vec3d diff{ip->getPosition() - ownship->getPosition()};
        rng = diff.length();
    }
    return rng;
}

//------------------------------------------------------------------------------
// newModelEntry() -- Generates a new model entry for this player.
//                    Returns a pointer to the new entry, else zero(0)
//------------------------------------------------------------------------------
CigiModel* IgHost::newModelEntry(models::Player* const ip)
{
   CigiModel* model{};

   // Only if we have a player pointer ...
   if (ip != nullptr && (getModelTableSize() < getMaxModels())) {

      // Create a model entry for this player
      model = modelFactory();
      if (model != nullptr) {
         // Yes, initialize the model entry
         model->initialize(ip, otwModelTypes.data(), nOtwModelTypes);
         addModelToList(model, MODEL_TABLE);
      }
   }
   return model;
}

//------------------------------------------------------------------------------
// newElevEntry() -- Generates a new elevation entry for this player
//                    Returns a pointer to the new entry, else zero(0)
//------------------------------------------------------------------------------
CigiModel* IgHost::newElevEntry(models::Player* const ip)
{
   CigiModel* model{};

   // Only if we have a player pointer ...
   if (ip != nullptr && (getElevationTableSize() < getMaxElevations())) {

      // Create a model entry for this player
      model = hotFactory();
      if (model != nullptr) {
         // Yes, initialize the model entry
         model->initialize(ip);
         addModelToList(model, HOT_TABLE);
      }
   }
   return model;
}

//------------------------------------------------------------------------------
// Sets our ownship pointer; public version, which is usually called by
// the Station class.  Derived versions of Otw can override this function
// and control the switch of ownship using setOwnship0()
//------------------------------------------------------------------------------
void IgHost::setOwnship(simulation::AbstractPlayer* const newOwnship)
{
   const auto player = dynamic_cast<models::Player*>(newOwnship);
   if (player != nullptr) {
      setOwnship0(player);
   }
}

//------------------------------------------------------------------------------
// Sets our ownship player (for derived class control)
//------------------------------------------------------------------------------
void IgHost::setOwnship0(models::Player* const newOwnship)
{
    // Nothing's changed, just return
    if (ownship == newOwnship) return;

    // Unref() the old, set and ref() the new
    if (ownship != nullptr) ownship->unref();
    ownship = newOwnship;
    if (ownship != nullptr) ownship->ref();
}

//------------------------------------------------------------------------------
// setPlayerList() -- Sets our player list pointer
//------------------------------------------------------------------------------
void IgHost::setPlayerList(base::PairStream* const newPlayerList)
{
    // Nothing's changed, just return
    if (playerList == newPlayerList) return;

    // Unref() the old, set and ref() the new
    if (playerList != nullptr) playerList->unref();
    playerList = newPlayerList;
    if (playerList != nullptr) playerList->ref();
}

//------------------------------------------------------------------------------
// setMaxRange() -- sets the max visual range
//------------------------------------------------------------------------------
bool IgHost::setMaxRange(const double r)
{
    maxRange = std::fabs(r);
    return true;
}

//------------------------------------------------------------------------------
// setMaxModels() -- sets the maximum number of models accepted by the visual
//------------------------------------------------------------------------------
bool IgHost::setMaxModels(const int n)
{
    if (n <= MAX_MODELS) maxModels = n;
    else maxModels = MAX_MODELS;
    resetTables();
    return true;
}

//------------------------------------------------------------------------------
// setMaxElevations() -- sets the maximum number of elevation requests accepted by the visual
//------------------------------------------------------------------------------
bool IgHost::setMaxElevations(const int n)
{
    if (n <= MAX_MODELS) maxElevations = n;
    else maxElevations = MAX_MODELS;
    resetTables();
    return true;
}

//------------------------------------------------------------------------------
// addModelToList() -- adds a model to the quick access table
//------------------------------------------------------------------------------
bool IgHost::addModelToList(CigiModel* const model, const TableType type)
{
   bool ok{};
   if (model != nullptr) {

      // Select the table
      CigiModel** tbl{modelTbl.data()};
      int n{nModels};
      int max{maxModels};
      if (type == HOT_TABLE) {
         tbl = hotTbl.data();
         n = nHots;
         max = maxElevations;
      }

      // If there's room for one more ...
      if (n < max) {

         // Put the model on the top of the table
         model->ref();
         tbl[n] = model;

         // Create a key for this new NIB
         ModelKey key(model->getPlayerID(), model->getFederateName());

         if (n > 0) {
            // Now, 'bubble down' to its correct position
            int idx{n-1};
            while (idx >= 0 && compareKey2Model(&key, &tbl[idx]) <= 0) {
               // Swap the table entries
               CigiModel* tmp{tbl[idx]};
               tbl[idx] = tbl[idx+1];
               tbl[idx+1] = tmp;
               idx--;
            }
         }

         // Increment the count
         if (type == HOT_TABLE) nHots++;
         else nModels++;

         ok = true;
      }
   }
   return ok;
}

//------------------------------------------------------------------------------
// removeModelFromList() -- removes a model from the quick access table
//------------------------------------------------------------------------------
void IgHost::removeModelFromList(const int idx, const TableType type)
{
   // Select the table size
   int n{nModels};
   if (type == HOT_TABLE) {
      n = nHots;
   }

   // If a valid index ...
   if (idx >= 0 && idx < n) {

      // Select the table
      CigiModel** tbl{modelTbl.data()};
      if (type == HOT_TABLE) {
         tbl = hotTbl.data();
      }

      // Remember the model
      CigiModel* model{tbl[idx]};

      // Shift down all items above this index by one position
      int n1{n - 1};
      for (int i = idx; i < n1; i++) {
         tbl[i] = tbl[i+1];
      }

      // Decrement the count
      if (type == HOT_TABLE) --nHots;
      else --nModels;

      // clear the last pointer
      tbl[n-1] = nullptr;

      // Unref the model
      model->unref();
   }
}

void IgHost::removeModelFromList(CigiModel* const model, const TableType type)
{
   CigiModel** tbl{modelTbl.data()};
   int n{nModels};
   if (type == HOT_TABLE) {
      tbl = hotTbl.data();
      n = nHots;
   }

   int found{-1};
   // Find the model
   for (int i = 0; i < n && found < 0; i++) {
      if (model == tbl[i]) found = i;
   }

   // If the entry was found ...
   if (found >= 0) {

      // Shift down all items above this model by one position
      int n1{n - 1};
      for (int i = found; i < n1; i++) {
         tbl[i] = tbl[i+1];
      }

      // Decrement the count
      if (type == HOT_TABLE) --nHots;
      else --nModels;

      // clear the last pointer
      tbl[n-1] = nullptr;

      // Unref the model
      model->unref();
   }
}

//------------------------------------------------------------------------------
// findModel() -- find the model that matches ALL IDs.
//------------------------------------------------------------------------------
CigiModel* IgHost::findModel(const unsigned short playerID, const base::String* const federateName, const TableType type)
{
   // Define the key
   ModelKey key(playerID, federateName);

   // Binary search the table for the models
   CigiModel* found{};
   if (type == HOT_TABLE) {
      CigiModel** k{static_cast<CigiModel**>(bsearch(&key, hotTbl.data(), nHots, sizeof(CigiModel*), compareKey2Model))};
      if (k != nullptr) found = *k;
   } else {
      CigiModel** k{static_cast<CigiModel**>(bsearch(&key, modelTbl.data(), nModels, sizeof(CigiModel*), compareKey2Model))};
      if (k != nullptr) found = *k;
   }
   return found;
}

CigiModel* IgHost::findModel(const simulation::AbstractPlayer* const player, const TableType type)
{
   CigiModel* found{};
   if (player != nullptr) {
      // Get the player's IDs
      const base::String* fName{};
      if (player->isNetworkedPlayer()) {
         // If networked, used original IDs
         const simulation::AbstractNib* pNib{player->getNib()};
         fName = pNib->getFederateName();
      }
      // Now find the model using the player's IDs
      found = findModel(player->getID(), fName, type);
   }
   return found;
}

//------------------------------------------------------------------------------
// bsearch callbacks: object name compare function --
//   True types are (const OtwModelKey* key, const OtwModel** model)
//------------------------------------------------------------------------------
int IgHost::compareKey2Model(const void* key, const void* model)
{
   // The Key
   const ModelKey* pKey{static_cast<const ModelKey*>(key)};

   // The NIB
   const CigiModel* const* pp{static_cast<const CigiModel* const*>(model)};
   const CigiModel* pModel{*pp};

   // Default to equal
   int result{};

   // Compare player IDs
   if (pKey->playerID < pModel->getPlayerID()) result = -1;
   else if (pKey->playerID > pModel->getPlayerID()) result = +1;

   if (result == 0) {
      // If they're the same playr IDs, compare the federate names
      const base::String* pKeyFedName{pKey->fName};
      const base::String* pModelFedName{pModel->getFederateName()};

      if (pKeyFedName == nullptr && pModelFedName != nullptr) result = -1;

      else if (pKeyFedName != nullptr && pModelFedName == nullptr) result = +1;

      else if (pKeyFedName != nullptr && pModelFedName != nullptr) {
         result = std::strcmp(*pKeyFedName, *pModelFedName);
      }
   }

   return result;
}

//------------------------------------------------------------------------------
// True if visual system is resetting
//------------------------------------------------------------------------------
bool IgHost::isResetInProgress() const
{
   return rstFlg;
}

//------------------------------------------------------------------------------
// Data set routines
//------------------------------------------------------------------------------
bool IgHost::setRefLatitude(const double v)
{
    bool ok{v <= 90.0 && v >= -90.0};
    if (ok) refLat = v;
    return ok;
}

bool IgHost::setRefLongitude(const double v)
{
    bool ok{v <= 180.0 && v >= -180.0};
    if (ok) refLon = v;
    return ok;
}

//------------------------------------------------------------------------------
// Set Slot Functions
//------------------------------------------------------------------------------

// setSlotMaxRange() -- sets the maxRange slot
bool IgHost::setSlotMaxRange(const base::Distance* const msg)
{
    bool ok{};

    if (msg != nullptr) {
        // We have a distance which we can convert to meters
        const double rng{base::Meters::convertStatic(*msg)};
        ok = setMaxRange( rng );
    }

    if (!ok) {
        // error -- invalid range
        std::cerr << "IgHost::setSlotMaxRange: invalid maximum range" << std::endl;
    }
    return ok;
}

// setSlotMaxRange() -- sets the maxRange slot
bool IgHost::setSlotMaxRange(const base::Number* const msg)
{
    bool ok{};

    if (msg != nullptr) {
        // We have a simple number, which should be meters!
        ok = setMaxRange(msg->getReal());
    }

    if (!ok) {
        // error -- invalid range
        std::cerr << "IgHost::setSlotMaxRange: invalid maximum range" << std::endl;
    }
    return ok;
}

// setSlotMaxModels() -- sets the max number of models slot
bool IgHost::setSlotMaxModels(const base::Number* const num)
{
    bool ok{};
    if (num != nullptr) {
        const int n{num->getInt()};
        if (n >= 0) {
             ok = setMaxModels(n);
        }
        if (!ok) {
            std::cerr << "IgHost::setSlotMaxModels: maximum number of models limited to Otw::MAX_MODELS1" << std::endl;
        }
    }
    return ok;
}

bool IgHost::setSlotMaxElevations(const base::Number* const num)
{
    bool ok{};
    if (num != nullptr) {
        const int n{num->getInt()};
        if (n >= 0) {
             ok = setMaxElevations(n);
        }
        if (!ok) {
            std::cerr << "IgHost::setSlotMaxElevations: maximum number of terrain elevation requests limited to Otw::MAX_ELEV" << std::endl;
        }
    }
    return ok;
}

// latitude: Ref latitude (deg)
bool IgHost::setSlotRefLatitude(const base::Number* const num)
{
    bool ok{};
    if (num != nullptr) {
        ok = setRefLatitude(num->getDouble());
    }
    return ok;
}

// longitude: Ref longitude (deg)
bool IgHost::setSlotRefLongitude(const base::Number* const num)
{
    bool ok{};
    if (num != nullptr) {
        ok = setRefLongitude(num->getDouble());
    }
    return ok;
}

// Sets the list of IG model type IDs (TypeMapper objects)
bool IgHost::setSlotTypeMap(const base::PairStream* const msg)
{
    bool ok{};
    if (msg != nullptr) {
       // First clear the old table
       clearModelTypes();

       // Now scan the pair stream and put all Otm objects
       // into the table.
       const base::List::Item* item{msg->getFirstItem()};
       while (item != nullptr && nOtwModelTypes < MAX_MODELS_TYPES) {
          const auto pair = static_cast<const base::Pair*>(item->getValue());
          const auto otwType = dynamic_cast<const TypeMapper*>( pair->object() );
          if (otwType != nullptr) {
             // We have an Otm object, so put it in the table
             otwType->ref();
             otwModelTypes[nOtwModelTypes] = otwType;
             nOtwModelTypes++;
          }
          item = item->getNext();
       }
       ok = true;
    }
    return ok;
}

//==============================================================================
// IgModel::ModelKey class
//==============================================================================
IgHost::ModelKey::ModelKey(const unsigned short pid, const base::String* const federateName)
{
   playerID = pid;
   fName = federateName;
}

}
}