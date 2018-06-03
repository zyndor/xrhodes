--==============================================================================
--
-- XRhodes
--
-- copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
--
--==============================================================================
project "xr3"

	kind "StaticLib"
	
	files {
		"h/**.hpp",
		"src/*.cpp",
	}
	
	includedirs {
		"h/",
		"../xr3core/h/",
		"../xr3json/h/",
		"../external/tinyxml2",
		"../external/libpng",
		"../external/stb",
		"../external/utf8cpp",
	}
	
	if target_desktop then
		files {
			"src/SDL/*.hpp",
			"src/SDL/*.cpp",
		}
		
		includedirs {
			"../external/SDL2/include/",
		}
	end