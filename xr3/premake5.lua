--==============================================================================
--
-- XRhodes
--
-- copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
--
--==============================================================================
project "xr3.SDL"

	kind "StaticLib"
	
	files {
		"h/**.hpp",
		"src/*.cpp",
		"src/SDL/*.hpp",
		"src/SDL/*.cpp",
	}
	
	includedirs {
		"h/",
		"../xr3core/h/",
		"../xr3json/h/",
		"../external/glew/include/",
		"../external/SDL2/include/",
		"../external/SDL2_image/include",
		"../external/SDL2_mixer/include",
		"../external/tinyxml2",
		"../external/libpng",
	}