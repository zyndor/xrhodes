--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
project "xr3json"

	kind "StaticLib"

	files {
		"h/**.hpp",
		"src/**.cpp",
		"premake5.lua"
	}

	includedirs {
		"h/",
		"../xr3core/h/",
	}