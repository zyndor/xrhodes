--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
project "xr3ui"

	kind "StaticLib"
	
	files {
		"h/**.hpp",
		"src/**.cpp",
	}
	
	includedirs {
		"h/",
		"../xr3core/h/",
		"../xr3json/h/",
		"../xr3/h/",
		"../external/tinyxml2",
	}