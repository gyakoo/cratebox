-- WORK IN PROGRESS NOT USE --
local action = _ACTION or ""
local build="build"..action
solution "mspuzzle"
	location ( build )
	configurations { "Debug", "Release" }
	platforms {"x64"}
    buildoptions { "/MP8" }    
    
  	project "game"
		kind "WindowedApp"
        flags {"WinMain"}
		language "C++"
		files { "PuzzleGame/**.cpp", "PuzzleGame/**.h", "SDL2/**.h", "SDL2/**.c", "Box2D/**.h"}
		excludes { "SDL2/SDL2_gfx-1.0.1/test/*.*"}
		includedirs { "./", "PuzzleGame/", "SDL2/SDL2-2.0.4/include/", "SDL2/SDL2_gfx-1.0.1/", "SDL2/SDL2_image-2.0.1/include/", "SDL2/SDL2_ttf-2.0.14/include/" }		
	 	debugdir "./PuzzleGame/" 
        
		configuration { "windows" }
			links { "gdi32", "winmm", "user32", "SDL2.lib", "SDL2_image.lib", "SDL2_ttf.lib", "Box2D.lib"}
                        
        configuration { "Debug", "x64" }
            defines { "DEBUG", "_DEBUG" }
            flags { "Symbols", "ExtraWarnings"}
            libdirs {"SDL2/SDL2-2.0.4/lib/x64/", "SDL2/SDL2_image-2.0.1/lib/x64/", "SDL2/SDL2_ttf-2.0.14/lib/x64/", "Box2D/lib/x64/Debug/"}
            objdir (build.."/obj/x64/debug")
            targetdir (build.."/bin/x64/debug/")

        configuration {"Release", "x64"}
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            libdirs {"SDL2/SDL2-2.0.4/lib/x64/", "SDL2/SDL2_image-2.0.1/lib/x64/", "SDL2/SDL2_ttf-2.0.14/lib/x64/", "Box2D/lib/x64/Release/"}
            objdir (build.."/obj/x64/release" )   
            targetdir (build.."/bin/x64/release/")    
			
			
			
	project "testgfx"
		kind "ConsoleApp"
		language "C++"
		files { "SDL2/**.h", "SDL2/**.c"}
		--excludes { "SDL2/SDL2_gfx-1.0.1/test/testimagefilter.c", "SDL2/SDL2_gfx-1.0.1/test/testrotozoom.c"}
		includedirs { "./", "SDL2/SDL2-2.0.4/include/", "SDL2/SDL2_gfx-1.0.1/", "SDL2/SDL2_image-2.0.1/include/", "SDL2/SDL2_ttf-2.0.14/include/" }		
	 	debugdir "./PuzzleGame/" 
        
		configuration { "windows" }
			links { "gdi32", "winmm", "user32", "SDL2.lib", "SDL2Test.lib", "SDL2Main.lib",  "SDL2_image.lib", "SDL2_ttf.lib" }
                        
        configuration { "Debug", "x64" }
            defines { "DEBUG", "_DEBUG" }
            flags { "Symbols", "ExtraWarnings"}
            libdirs {"SDL2/SDL2-2.0.4/lib/x64/", "SDL2/SDL2_image-2.0.1/lib/x64/", "SDL2/SDL2_ttf-2.0.14/lib/x64/"}
            objdir (build.."/obj/x64/debug")
            targetdir (build.."/bin/x64/debug/")

        configuration {"Release", "x64"}
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            libdirs {"SDL2/SDL2-2.0.4/lib/x64/", "SDL2/SDL2_image-2.0.1/lib/x64/", "SDL2/SDL2_ttf-2.0.14/lib/x64/"}
            objdir (build.."/obj/x64/release" )   
            targetdir (build.."/bin/x64/release/")