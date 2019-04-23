--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
project "xr3scene"

	kind "StaticLib"

	files {
		"h/**.hpp",
		"src/**.cpp",
		"premake5.lua"
	}

	if (is_vs()) then
		files { "xr3scene.natvis"}
	end

	includedirs {
		"h/",
		"../xr3core/h/",
		"../xr3json/h/",
		"../xr3/h/",
	}
