
--
-- If premake command is not supplied an action (target compiler), exit!
--
-- Targets of interest:
--     vs2013     (Visual Studio 2013)
--     vs2015     (Visual Studio 2015)
--     vs2017     (Visual Studio 2017)
--
if (_ACTION == nil) then
    return
end

--
-- directory location for 3rd party dependencies
--
MXRP_3RD_PARTY_ROOT = "../../../mxrp-3rdparty"

--
-- set include and library paths
--
MXRP_IncPath         = "../../include"
MXRP_3rdPartyIncPath = MXRP_3RD_PARTY_ROOT.."/include"

--
-- directory location for HLA include and library paths
--
HLA_ROOT = "../../../openrti"
HLAIncPath = HLA_ROOT.."/include/RTI13"
HLALibPath = HLA_ROOT.."/lib"
print ("HLA Paths:")
print ("  Include   : "..HLALibPath)
--print ("  Libraries : "..MXRP_LibPath)

workspace "mxrp"

   -- destination directory for generated solution/project files
   location ("../" .. _ACTION)

   -- destination directory for compiled binary target
   targetdir ("../../lib/")

   -- creating static libraries
   kind "StaticLib"

   -- C++ code in all projects
   language "C++"

   -- common include directories (all configurations/all projects)
   includedirs { MXRP_IncPath, MXRP_3rdPartyIncPath }

   -- target suffix (all configurations/all projects)
   targetprefix "mxrp_"

   --
   -- Build (solution) configuration options:
   --     Release        (Runtime library is Multi-threaded DLL)
   --     Debug          (Runtime library is Multi-threaded Debug DLL)
   --
   configurations { "Release", "Debug" }

   -- visual studio options and warnings
   -- /wd4351 (C4351 warning) - disable warning associated with array brace initialization
   -- /wd4996 (C4996 warning) - disable deprecated declarations
   -- /wd4005 (C4005 warning) - disable macro redefinition
   -- /wd4100 (C4100 warning) - disable unreferenced formal parameter
   -- /Oi - generate intrinsic functions
   buildoptions( { "/wd4351", "/wd4996", "/wd4005", "/wd4100", "/Oi" } )

   -- common release configuration flags and symbols
   filter { "Release" }
      flags { "Optimize" }
      -- favor speed over size
      buildoptions { "/Ot" }
      defines { "WIN32", "_LIB", "NDEBUG" }

   -- common debug configuration flags and symbols
   filter { "Debug" }
      targetsuffix "_d"
      symbols "On"
      -- enable compiler intrinsics
      defines { "WIN32", "_LIB", "_DEBUG" }

   --
   -- libraries
   --

   -- base library
   project "base"
      files {
         "../../include/mxrp/base/**.h*",
         "../../include/mxrp/base/**.inl",
         "../../include/mxrp/base/**.epp",
         "../../include/mxrp/base/osg/*",
         "../../src/base/**.cpp",
         "../../src/base/**.y",
         "../../src/base/**.l"
      }
      excludes {
         "../../src/base/osg/Matrix_implementation.cpp",
         "../../src/base/util/platform/system_linux.cpp",
         "../../src/base/util/platform/system_mingw.cpp",
         "../../src/base/concurrent/platform/*_linux.cpp"
      }
      targetname "base"

   -- OpenGL-based graphics library
   project "graphics"
      files {
         "../../include/mxrp/graphics/**.h*",
         "../../src/graphics/**.cpp",
         "../../src/graphics/**.l"
      }
      includedirs { MXRP_3rdPartyIncPath.."/freetype2" }
      defines { "FTGL_LIBRARY_STATIC" }
      targetname "graphics"

   -- OpenGL GLUT interface library
   project "gui_glut"
      files {
         "../../include/mxrp/gui/glut/**.h*",
         "../../src/gui/glut/**.cpp"
      }
      targetname "gui_glut"

   -- DAFIF airport loader library
   project "dafif"
      files {
         "../../include/mxrp/dafif/**.h*",
         "../../src/dafif/**.cpp"
      }
      targetname "dafif"
	  
   -- graphical instruments library
   project "instruments"
      files {
         "../../include/mxrp/instruments/**.h*",
         "../../include/mxrp/instruments/**.epp",
         "../../src/instruments/**.cpp"
      }
      targetname "instruments"

   -- i/o device library
   project "iodevice"
      files {
         "../../include/mxrp/iodevice/**.h*",
         "../../src/iodevice/**.*"
      }
      excludes { "../../src/iodevice/platform/UsbJoystick_linux.*"   }
      targetname "iodevice"

   -- linear systems library
   project "linearsystem"
      files {
         "../../include/mxrp/linearsystem/**.h*",
         "../../src/linearsystem/**.cpp"
      }
      targetname "linearsystem"

   -- models library
   project "models"
      files {
         "../../include/mxrp/models/**.h*",
         "../../include/mxrp/models/**.inl",
         "../../src/models/**.cpp"
      }
      includedirs { MXRP_3rdPartyIncPath.."/JSBSim" }
      targetname "models"

   -- otw library
   project "otw"
      files {
         "../../include/mxrp/otw/**.h*",
         "../../src/otw/**.h*",
         "../../src/otw/**.cpp"
      }
      targetname "otw"

   project "recorder"
      files {
         "../../include/mxrp/recorder/**.h*",
         "../../include/mxrp/recorder/*.inl",
         "../../include/mxrp/recorder/**.proto",
         "../../src/recorder/**.cpp",
         "../../src/recorder/**.cc"
      }
      defines { "_SCL_SECURE_NO_WARNINGS" } -- suppress protocol buffer warning
      targetname "recorder"

   -- raster product format map library
   project "map_rpf"
      files {
         "../../include/mxrp/map/rpf/**.h*",
         "../../src/map/rpf/**.cpp"
      }
      targetname "map_rpf"

   -- simulation library
   project "simulation"
      files {
         "../../include/mxrp/simulation/**.h*",
         "../../include/mxrp/simulation/**.inl",
         "../../src/simulation/**.cpp"
      }
      targetname "simulation"

   -- terrain library
   project "terrain"
      files {
         "../../include/mxrp/terrain/**.h*",
         "../../src/terrain/**.cpp"
      }
      targetname "terrain"

   -- interoperability libraries
   dofile "interop.lua"
