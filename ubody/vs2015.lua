
local action = _ACTION or ""

solution "ubody"
	location ( "vs2015" )
	configurations { "Debug", "Release" }
	platforms {"x64", "x32"}
    startproject "only_rest"
    			
	-- //////////////////////// REST /////////////////////////////////
    project "ubody_rest" 
        kind "ConsoleApp"
        language "C++"
        files { "ubody_rest.cpp",  "ubody.hpp", "restserv.hpp", "contrib/**.h", "contrib/**.cpp", "contrib/**.inl" }
        includedirs { "./", "contrib/gason/" }      
        debugdir "./"
        
        configuration { "windows" }
            links { "gdi32", "winmm", "user32", "Ws2_32" }
            
        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols", "ExtraWarnings"}
            targetdir "vs2015/ubody_rest/debug/"

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            targetdir "vs2015/ubody_rest/release/"
            
    -- //////////////////////// only REST /////////////////////////////////
    project "only_rest" 
        kind "ConsoleApp"
        language "C++"
        files { "only_rest.cpp",  "restserv.hpp", "contrib/**.h", "contrib/**.cpp", "contrib/**.inl" }
        includedirs { "./", "contrib/gason" }
        debugdir "./"
        
        configuration { "windows" }
            links { "gdi32", "winmm", "user32", "Ws2_32" }
            
        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols", "ExtraWarnings"}
            targetdir "vs2015/only_rest/debug/"

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            targetdir "vs2015/only_rest/release/"