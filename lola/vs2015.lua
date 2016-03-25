
local action = _ACTION or ""

solution "lola_solution"
	location ( "vs2015" )
	configurations { "Debug", "Release" }
	platforms {"x64"}
    startproject "lolaGame"
    			
	-- //////////////////////// LOLA GAME /////////////////////////////////
    project "lolaGame" 
        kind "WindowedApp"
        language "C++"
        files { "lola/**.cpp", "lola/**.h", "ogresdk/**.h*" }
        includedirs { "./", "lola/", "ogresdk/include/", "ogresdk/include/OGRE/" }
        
        --configuration { "windows" }
            --links { "gdi32", "winmm", "user32", "Ws2_32" }
            
        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols", "ExtraWarnings", "FatalWarnings", "EnableSSE2", "WinMain"}
            targetdir "bin/debug/"
            libdirs { "ogresdk/lib/debug/", "ogresdk/lib/debug/opt/" }
            debugdir "bin/debug/"            
            links { "OgreMain_d.lib", "OgreMeshLodGenerator_d.lib", "OgreOverlay_d.lib", "OIS_d.lib", "Plugin_OctreeSceneManager_d.lib", "RenderSystem_Direct3D11_d.lib" }

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings", "FatalWarnings", "EnableSSE2", "WinMain"}
            targetdir "bin/release/"
            libdirs { "ogresdk/lib/release/", "ogresdk/lib/release/opt/" }
            debugdir "bin/release/"
            links { "OgreMain.lib", "OgreMeshLodGenerator.lib", "OgreOverlay.lib", "OIS.lib", "Plugin_OctreeSceneManager.lib", "RenderSystem_Direct3D11.lib" }