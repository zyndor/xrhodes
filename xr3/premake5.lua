--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
project "xr3"

	kind "StaticLib"

	files {
		"h/**.hpp",
		"src/*.cpp",
		"src/*.hpp",
		"premake5.lua"
	}

	if (is_vs()) then
		files { "xr3.natvis"}
	end

	includedirs {
		"h/",
		"../xr3core/h/",
		"../xr3json/h/",
		"../external/stb",
		"../external/utf8cpp",
	}

	if target_desktop then
		files {
			"src/GL/*.hpp",
			"src/GL/*.cpp",
			"src/SDL/*.hpp",
			"src/SDL/*.cpp",
		}

		includedirs {
			"src/",
			"src/SDL/",	-- NOTE: for access to GfxContext.hpp; for some reason filter{ "files:src/GL/Gfx.cpp" } didn't work.
		}
	end

