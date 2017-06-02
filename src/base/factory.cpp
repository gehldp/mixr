
#include "mxrp/base/factory.hpp"

#include "mxrp/base/Object.hpp"

#include "mxrp/base/FileReader.hpp"
#include "mxrp/base/Statistic.hpp"
#include "mxrp/base/Transforms.hpp"
#include "mxrp/base/Timers.hpp"

// Functors
#include "mxrp/base/functors/Tables.hpp"
#include "mxrp/base/functors/Functions.hpp"

// Numbers
#include "mxrp/base/Boolean.hpp"
#include "mxrp/base/Complex.hpp"
#include "mxrp/base/Integer.hpp"
#include "mxrp/base/Float.hpp"
#include "mxrp/base/Decibel.hpp"
#include "mxrp/base/LatLon.hpp"
#include "mxrp/base/Operators.hpp"

// IO
#include "mxrp/base/io/IoData.hpp"
#include "mxrp/base/io/IoHandler.hpp"

// Network handlers
#include "mxrp/base/network/TcpHandler.hpp"
#include "mxrp/base/network/TcpClient.hpp"
#include "mxrp/base/network/TcpServerMultiple.hpp"
#include "mxrp/base/network/TcpServerSingle.hpp"
#include "mxrp/base/network/UdpBroadcastHandler.hpp"
#include "mxrp/base/network/UdpMulticastHandler.hpp"
#include "mxrp/base/network/UdpUnicastHandler.hpp"

// Colors
#include "mxrp/base/Cie.hpp"
#include "mxrp/base/Cmy.hpp"
#include "mxrp/base/Hls.hpp"
#include "mxrp/base/Hsv.hpp"
#include "mxrp/base/Hsva.hpp"
#include "mxrp/base/Rgb.hpp"
#include "mxrp/base/Rgba.hpp"
#include "mxrp/base/Yiq.hpp"

// Units
#include "mxrp/base/units/AngularVelocity.hpp"
#include "mxrp/base/units/Angles.hpp"
#include "mxrp/base/units/Areas.hpp"
#include "mxrp/base/units/Distances.hpp"
#include "mxrp/base/units/Energies.hpp"
#include "mxrp/base/units/Forces.hpp"
#include "mxrp/base/units/Frequencies.hpp"
#include "mxrp/base/units/LinearVelocity.hpp"
#include "mxrp/base/units/Masses.hpp"
#include "mxrp/base/units/Powers.hpp"
#include "mxrp/base/units/Times.hpp"

// Random number generator and distributions
#include "mxrp/base/Rng.hpp"
#include "mxrp/base/distributions/Exponential.hpp"
#include "mxrp/base/distributions/Lognormal.hpp"
#include "mxrp/base/distributions/Pareto.hpp"
#include "mxrp/base/distributions/Uniform.hpp"

// Others
#include "mxrp/base/EarthModel.hpp"
#include "mxrp/base/concurrent/ThreadPool.hpp"

// Ubf
#include "mxrp/base/ubf/Agent.hpp"
#include "mxrp/base/ubf/Arbiter.hpp"

#include <string>

namespace oe {
namespace base {

Object* factory(const std::string& name)
{
    Object* obj = nullptr;

    // Numbers
    if ( name == Number::getFactoryName() ) {
        obj = new Number();
    }
    else if ( name == Complex::getFactoryName() ) {
        obj = new Complex();
    }
    else if ( name == Integer::getFactoryName() ) {
        obj = new Integer();
    }
    else if ( name == Float::getFactoryName() ) {
        obj = new Float();
    }
    else if ( name == Boolean::getFactoryName() ) {
        obj = new Boolean();
    }
    else if ( name == Decibel::getFactoryName() ) {
        obj = new Decibel();
    }
    else if ( name == LatLon::getFactoryName() ) {
        obj = new LatLon();
    }
    else if ( name == Add::getFactoryName() ) {
        obj = new Add();
    }
    else if ( name == Subtract::getFactoryName() ) {
        obj = new Subtract();
    }
    else if ( name == Multiply::getFactoryName() ) {
        obj = new Multiply();
    }
    else if ( name == Divide::getFactoryName() ) {
        obj = new Divide();
    }

    // Components
    else if ( name == FileReader::getFactoryName() ) {
        obj = new FileReader();
    }
    else if ( name == Statistic::getFactoryName() ) {
        obj = new Statistic();
    }

    // Transformations
    else if ( name == Translation::getFactoryName() ) {
        obj = new Translation();
    }
    else if ( name == Rotation::getFactoryName() ) {
        obj = new Rotation();
    }
    else if ( name == Scale::getFactoryName() ) {
        obj = new Scale();
    }

    // Functors
    else if ( name == Func1::getFactoryName() ) {
        obj = new Func1();
    }
    else if ( name == Func2::getFactoryName() ) {
        obj = new Func2();
    }
    else if ( name == Func3::getFactoryName() ) {
        obj = new Func3();
    }
    else if ( name == Func4::getFactoryName() ) {
        obj = new Func4();
    }
    else if ( name == Func5::getFactoryName() ) {
        obj = new Func5();
    }
    else if ( name == Polynomial::getFactoryName() ) {
        obj = new Polynomial();
    }
    else if ( name == Table1::getFactoryName() ) {
        obj = new Table1();
    }
    else if ( name == Table2::getFactoryName() ) {
        obj = new Table2();
    }
    else if ( name == Table3::getFactoryName() ) {
        obj = new Table3();
    }
    else if ( name == Table4::getFactoryName() ) {
        obj = new Table4();
    }
    else if ( name == Table5::getFactoryName() ) {
        obj = new Table5();
    }

    // Timers
    else if ( name == UpTimer::getFactoryName() ) {
        obj = new UpTimer();
    }
    else if ( name == DownTimer::getFactoryName() ) {
        obj = new DownTimer();
    }

    // Units: Angles
    else if ( name == Degrees::getFactoryName() ) {
        obj = new Degrees();
    }
    else if ( name == Radians::getFactoryName() ) {
        obj = new Radians();
    }
    else if ( name == Semicircles::getFactoryName() ) {
        obj = new Semicircles();
    }

    // Units: Areas
    else if ( name == SquareMeters::getFactoryName() ) {
        obj = new SquareMeters();
    }
    else if ( name == SquareFeet::getFactoryName() ) {
        obj = new SquareFeet();
    }
    else if ( name == SquareInches::getFactoryName() ) {
        obj = new SquareInches();
    }
    else if ( name == SquareYards::getFactoryName() ) {
        obj = new SquareYards();
    }
    else if ( name == SquareMiles::getFactoryName() ) {
        obj = new SquareMiles();
    }
    else if ( name == SquareCentiMeters::getFactoryName() ) {
        obj = new SquareCentiMeters();
    }
    else if ( name == SquareMilliMeters::getFactoryName() ) {
        obj = new SquareMilliMeters();
    }
    else if ( name == SquareKiloMeters::getFactoryName() ) {
        obj = new SquareKiloMeters();
    }
    else if ( name == DecibelSquareMeters::getFactoryName() ) {
        obj = new DecibelSquareMeters();
    }

    // Units: Distances
    else if ( name == Meters::getFactoryName() ) {
        obj = new Meters();
    }
    else if ( name == CentiMeters::getFactoryName() ) {
        obj = new CentiMeters();
    }
    else if ( name == MicroMeters::getFactoryName() ) {
        obj = new MicroMeters();
    }
    else if ( name == Microns::getFactoryName() ) {
        obj = new Microns();
    }
    else if ( name == KiloMeters::getFactoryName() ) {
        obj = new KiloMeters();
    }
    else if ( name == Inches::getFactoryName() ) {
        obj = new Inches();
    }
    else if ( name == Feet::getFactoryName() ) {
        obj = new Feet();
    }
    else if ( name == NauticalMiles::getFactoryName() ) {
        obj = new NauticalMiles();
    }
    else if ( name == StatuteMiles::getFactoryName() ) {
        obj = new StatuteMiles();
    }

    // Units: Energies
    else if ( name == KiloWattHours::getFactoryName() ) {
        obj = new KiloWattHours();
    }
    else if ( name == BTUs::getFactoryName() ) {
        obj = new BTUs();
    }
    else if ( name == Calories::getFactoryName() ) {
        obj = new Calories();
    }
    else if ( name == FootPounds::getFactoryName() ) {
        obj = new FootPounds();
    }
    else if ( name == Joules::getFactoryName() ) {
        obj = new Joules();
    }

    // Units: Forces
    else if ( name == Newtons::getFactoryName() ) {
        obj = new Newtons();
    }
    else if ( name == KiloNewtons::getFactoryName() ) {
        obj = new KiloNewtons();
    }
    else if ( name == Poundals::getFactoryName() ) {
        obj = new Poundals();
    }
    else if ( name == PoundForces::getFactoryName() ) {
        obj = new PoundForces();
    }

    // Units: Frequencies
    else if ( name == Hertz::getFactoryName() ) {
        obj = new Hertz();
    }
    else if ( name == KiloHertz::getFactoryName() ) {
        obj = new KiloHertz();
    }
    else if ( name == MegaHertz::getFactoryName() ) {
        obj = new MegaHertz();
    }
    else if ( name == GigaHertz::getFactoryName() ) {
        obj = new GigaHertz();
    }
    else if ( name == TeraHertz::getFactoryName() ) {
        obj = new TeraHertz();
    }

    // Units: Masses
    else if ( name == Grams::getFactoryName() ) {
        obj = new Grams();
    }
    else if ( name == KiloGrams::getFactoryName() ) {
        obj = new KiloGrams();
    }
    else if ( name == Slugs::getFactoryName() ) {
        obj = new Slugs();
    }

    // Units: Powers
    else if ( name == KiloWatts::getFactoryName() ) {
        obj = new KiloWatts();
    }
    else if ( name == Watts::getFactoryName() ) {
        obj = new Watts();
    }
    else if ( name == MilliWatts::getFactoryName() ) {
        obj = new MilliWatts();
    }
    else if ( name == Horsepower::getFactoryName() ) {
        obj = new Horsepower();
    }
    else if ( name == DecibelWatts::getFactoryName() ) {
        obj = new DecibelWatts();
    }
    else if ( name == DecibelMilliWatts::getFactoryName() ) {
        obj = new DecibelMilliWatts();
    }

    // Units: Time
    else if ( name == Seconds::getFactoryName() ) {
        obj = new Seconds();
    }
    else if ( name == MilliSeconds::getFactoryName() ) {
        obj = new MilliSeconds();
    }
    else if ( name == MicroSeconds::getFactoryName() ) {
        obj = new MicroSeconds();
    }
    else if ( name == NanoSeconds::getFactoryName() ) {
        obj = new NanoSeconds();
    }
    else if ( name == Minutes::getFactoryName() ) {
        obj = new Minutes();
    }
    else if ( name == Hours::getFactoryName() ) {
        obj = new Hours();
    }
    else if ( name == Days::getFactoryName() ) {
        obj = new Days();
    }

    // Units: Velocities
    else if ( name == AngularVelocity::getFactoryName() ) {
        obj = new AngularVelocity();
    }
    else if ( name == LinearVelocity::getFactoryName() ) {
        obj = new LinearVelocity();
    }

    // Colors
    else if ( name == Color::getFactoryName() ) {
        obj = new Color();
    }
    else if ( name == Cie::getFactoryName() ) {
        obj = new Cie();
    }
    else if ( name == Cmy::getFactoryName() ) {
        obj = new Cmy();
    }
    else if ( name == Hls::getFactoryName() ) {
        obj = new Hls();
    }
    else if ( name == Hsv::getFactoryName() ) {
        obj = new Hsv();
    }
    else if ( name == Hsva::getFactoryName() ) {
        obj = new Hsva();
    }
    else if ( name == Rgb::getFactoryName() ) {
        obj = new Rgb();
    }
    else if ( name == Rgba::getFactoryName() ) {
        obj = new Rgba();
    }
    else if ( name == Yiq::getFactoryName() ) {
        obj = new Yiq();
    }

    // Network handlers
    else if ( name == TcpClient::getFactoryName() ) {
        obj = new TcpClient();
    }
    else if ( name == TcpServerSingle::getFactoryName() ) {
        obj = new TcpServerSingle();
    }
    else if ( name == TcpServerMultiple::getFactoryName() ) {
        obj = new TcpServerMultiple();
    }
    else if ( name == UdpBroadcastHandler::getFactoryName() ) {
        obj = new UdpBroadcastHandler();
    }
    else if ( name == UdpMulticastHandler::getFactoryName() ) {
        obj = new UdpMulticastHandler();
    }
    else if ( name == UdpUnicastHandler::getFactoryName() ) {
        obj = new UdpUnicastHandler();
    }

    // Random number generator and distributions
    else if ( name == Rng::getFactoryName() ) {
        obj = new Rng();
    }
    else if ( name == Exponential::getFactoryName() ) {
        obj = new Exponential();
    }
    else if ( name == Lognormal::getFactoryName() ) {
        obj = new Lognormal();
    }
    else if ( name == Pareto::getFactoryName() ) {
        obj = new Pareto();
    }
    else if ( name == Uniform::getFactoryName() ) {
        obj = new Uniform();
    }

    // General I/O Devices
    else if ( name == IoHandler::getFactoryName() ) {
        obj = new IoHandler();
    }
    else if ( name == IoData::getFactoryName() ) {
        obj = new IoData();
    }

    // Earth models
    else if ( name == EarthModel::getFactoryName() ) {
        obj = new EarthModel();
    }

    // Thread pool
    else if ( name == ThreadPool::getFactoryName() ) {
       obj = new ThreadPool();
    }

    // Ubf
    else if ( name == ubf::Agent::getFactoryName() ) {
        obj = new ubf::Agent();
    }
    else if ( name == ubf::Arbiter::getFactoryName() ) {
        obj = new ubf::Arbiter();
    }

    return obj;
}

}
}

