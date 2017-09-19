project "unittests"

	kind "ConsoleApp"
	
	files
	{
		"h/**.hpp",
		"src/**.cpp",
	}
	
	includedirs
	{
		"../external/gtest/include",
		"../unittests",
		"../xr3core/h",
		"../xr3json/h",
		"../xr3/h",
		"../xr3ui/h",
	}
	
	defines { "DATA_PATH=\""..path.getabsolute("./data").."\"" }
	
	links
	{
		"gtest",
		"gtest_main",
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
		"../external/gtest/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
	}
