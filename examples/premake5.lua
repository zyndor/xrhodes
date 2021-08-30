--==============================================================================
--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
project "examples"

	kind "ConsoleApp"

	files
	{
		"*.cpp",
		"*.hpp",
		"data/*",
		"premake5.lua"
	}

	includedirs
	{
		"../unittests/h",
		"../xr3core/h",
		"../xr3json/h",
		"../xr3/h",
		"../xr3scene/h",
	}

	defines { "DATA_PATH=\""..path.getabsolute("./data").."\"" }

	if is_msvc() then
		buildoptions { "/WX-" }
	else
		buildoptions { "-Wno-error" }
	end

	-- link options
	links
	{
		"tinyxml2",
		"SDL2",

		"xr3scene",
		"xr3",
		"xr3json",
		"xr3core",
	}

	if target_env == "windows" then
		-- Windows
		links
		{
			"libpng16",
			"zlib",
			"opengl32"
		}

		libdirs
		{
			"$(VcpkgCurrentInstalledDir)lib",
			"$(VcpkgCurrentInstalledDir)lib/manual-link",
		}

	else
		if target_env == "macos" then
			-- MacOS
			links
			{
				"OpenGL.framework"
			}

		else
			-- other *nix
			links
			{
				"GL",
			}

		end

		-- common *nix link options
		links
		{
			"png16",
			"z",
			"pthread"
		}

	end

	-- custom build step
	if target_env == "windows" then
		local dependencies = { "SDL2", "libpng16", "zlib1" }
		do_vs_postbuild(dependencies)
	end
