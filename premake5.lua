-- WORK IN PROGRESS NOT USE --
local action = _ACTION or ""
local build="build"..action
solution "mspuzzle"
	location ( build )
	configurations { "Debug", "Release" }
	platforms {"x64", "x32"}
    buildoptions { "/MP8" }    
    
  	project "mspuzzle"
		kind "WindowedApp"
        flags {"WinMain"}
		language "C++"
		files { "PuzzleGame/**.cpp", "PuzzleGame/**.h" }
		includedirs { "./", "PuzzleGame/", "SDL2-2.0.0/include/" }		
	 	debugdir "./PuzzleGame/" 
        
		configuration { "windows" }            
			links { "gdi32", "winmm", "user32", "SDL2.lib", "SDL2_image.lib", "SDL2_mixer.lib", "SDL2_ttf.lib" }
            
	    configuration { "Debug", "x32" }
            defines { "DEBUG", "_DEBUG" }
            flags { "Symbols", "ExtraWarnings"}                
            libdirs {"SDL2-2.0.0/lib/x86/"}
            objdir (build.."/obj/x32/debug")
            targetdir (build.."/bin/x32/debug/")
            
        configuration { "Debug", "x64" }
            defines { "DEBUG", "_DEBUG" }
            flags { "Symbols", "ExtraWarnings"}
            libdirs {"SDL2-2.0.0/lib/x64/"}
            objdir (build.."/obj/x64/debug")
            targetdir (build.."/bin/x64/debug/")

        configuration {"Release", "x32"}
            defines { "NDEBUG" }            
            flags { "Optimize", "ExtraWarnings"}
            libdirs {"SDL2-2.0.0/lib/x86/"}
            objdir (build.."/obj/x32/release")
            targetdir (build.."/bin/x32/release/")
            
        configuration {"Release", "x64"}
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            libdirs {"SDL2-2.0.0/lib/x64/"}
            objdir (build.."/obj/x64/release" )   
            targetdir (build.."/bin/x64/release/")    