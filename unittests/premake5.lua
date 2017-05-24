project "unittests"

	kind "SharedLib"
	
	pchheader "stdafx.h"
	pchsource "stdafx.cpp"
	
	files
	{
		"h/**.hpp",
		"src/**.cpp",
		"stdafx.*",
		"targetver.h",
	}
	
	includedirs
	{
		"../unittests",
		"../xr3core/h",
		"../xr3json/h",
		"../xr3/h",
		"../xr3ui/h",
		
		"$(VCInstallDir)/Unittest/include",
	}
	
	defines { "DATA_PATH=\""..path.getabsolute("./data").."\"" }
	
	links
	{
		"xr3core",
		"xr3json",
		"xr3.SDL",
		"xr3ui",
	}
	
	libdirs
	{
		"../external/tinyxml-2.6.2/lib/$(Platform)/Release",
		"../external/SDL2-2.0.4/lib/$(PlatformShortName)/",
		"../external/SDL2_image-2.0.1/lib/$(PlatformShortName)",
		"../external/SDL2_mixer-2.0.1/lib/$(PlatformShortName)",
		"../external/glew-2.0.0/lib/Release/$(PlatformShortName)",
		"$(VCInstallDir)UnitTest/lib",
	}
