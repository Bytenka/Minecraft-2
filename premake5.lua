workspace "Minecraft-3"
   configurations { "Debug", "Release" }

project "Minecraft-3"
   kind "ConsoleApp"
   language "C++"
   files { "src/**.h", "src/**.hpp", "src/**.c", "src/**.cpp"}

   pchheader "src/pchutils.h"
   pchsource "src/pchutils.cpp"
   includedirs {
       "src/",              -- Needed for precompiled headers
       "../../../../../../Dropbox/OpenGL/Libraries/Includes/*"
   }

   objdir ("obj")
   targetdir ("bin/%{cfg.buildcfg}")
   targetname "program.out"

   filter { "configurations:Debug" }
      defines { "DEBUG" }
      symbols "On"
      optimize "Off"

   filter { "configurations:Release" }
      defines { "NDEBUG" }
      optimize "On"