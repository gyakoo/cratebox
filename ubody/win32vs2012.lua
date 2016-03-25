
local action = _ACTION or ""

solution "ubody"
	location ( "win32vs2012" )
	configurations { "Debug", "Release" }
	platforms {"native", --[["x64",--]] "x32"}
    startproject "only_rest"
    
    -- //////////////////////// VDB /////////////////////////////////    
    local HAVOK_SDK = "E:/2014_2_5/"
    local HAVOK_SOURCE = HAVOK_SDK.."Source/"
    local HAVOK_LIB = HAVOK_SDK.."lib/win32_vs2012_win8/"
    project "ubody_vdb" 
        kind "ConsoleApp"
        language "C++"
        files { "ubody_vdb.cpp",  "ubody.hpp", "hkvdb.hpp", "contrib/**.h", "contrib/**.cpp", "contrib/**.inl" }
        includedirs { "./", "./contrib/DirectX/", HAVOK_SOURCE}      
        debugdir "./"
        
        configuration { "windows" }
            defines { "HK_TARGET_NAME=Win32Vs2012Win8" }
            links { "gdi32", "winmm", "user32",            
            "hkInternal", "hkGeometryUtilities", "hkImageUtilities",
            "hkVisualize", "hkSerialize", "hkCompat", "hkSceneData", "hkBase", "hkcdCollide", 
            "hkcdInternal" }
            links { "gdi32", "winmm", "user32" }
            
        configuration "Debug"
            defines { "DEBUG", "HK_DEBUG", "HK_DEBUG_SLOW" }
            flags { "Symbols", "ExtraWarnings"}
            libdirs { HAVOK_LIB.."debug/" }
            targetdir "win32vs2012/ubody_vdb/debug/"

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            libdirs { HAVOK_LIB.."release/" }
            targetdir "win32vs2012/ubody_vdb/release/"			
			
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
            targetdir "win32vs2012/ubody_rest/debug/"

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            targetdir "win32vs2012/ubody_rest/release/"
            
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
            targetdir "win32vs2012/only_rest/debug/"

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize", "ExtraWarnings"}
            targetdir "win32vs2012/only_rest/release/"