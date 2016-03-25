-- WORK IN PROGRESS NOT USE --
local action = _ACTION or ""

solution "nostalgia"
	location ( action or "build" )
	configurations { "Debug", "Release" }
	platforms {"x64", "x32"}
  
  --[[ 
      $(SolutionDir)thirdparty\vld\lib\Win32;
      $(SolutionDir)thirdparty\fmod\lib\;
      $(SolutionDir)thirdparty\box2d\lib\Win32\$(Configuration)\
  
    box2d.lib
    fmodL_vc.lib
    xinput.lib
    d3d11.lib
    d3dcompiler.lib
    d3dx11d.lib
    d3d9.lib
    dxerr.lib
    dxguid.lib
  --]]
  
	project "engine"
		kind "StaticLib"
		language "C++"
		files { "engine/**.h", "engine/**.cpp" }
		includedirs { "./", "engine/" }
		targetdir("bin")
        
		configuration { "windows" }            
			 links { "glu32","opengl32", "gdi32", "winmm", "user32" }
		
		configuration { "Debug", "x32"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}    