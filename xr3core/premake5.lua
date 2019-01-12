--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
project "xr3core"

	kind "StaticLib"

	files {
		"h/**.hpp",
		"src/**.cpp",
		"premake5.lua"
	}

	if (isVS()) then
		files { "xr3core.natvis"}
	end

	includedirs {
		"h/",
	}
