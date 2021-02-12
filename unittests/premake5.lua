--==============================================================================
--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
project "unittests"

	kind "ConsoleApp"

	files
	{
		"../external/xm/*.cpp",
		"h/**.hpp",
		"src/**.cpp",
		"data/**",
		"premake5.lua"
	}

	includedirs
	{
		"../external/libpng",
		"../external/xm",
		"../unittests/h",
		"../xr3core/h",
		"../xr3json/h",
		"../xr3/h",
		"../xr3scene/h",
	}

	defines { "DATA_PATH=\""..path.getabsolute("./data").."\"" }

	links
	{
		"xr3scene",
		"xr3",
		"xr3json",
		"xr3core",
	}

	if is_vs() then
		buildoptions { "/WX-" }
	else
		buildoptions { "-Wno-error" }
	end

	-- link options
	if target_env == "windows" then
		-- Windows
		links
		{
			"libpng16",
			"zlib",
			"opengl32",

			"SDL2",

			"tinyxml2",
		}

		libdirs
		{
			"../external/libpng/lib/"..target_env.."/$(PlatformShortName)-Release",
			"../external/tinyxml2/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
			"../external/SDL2/lib/"..target_env.."/$(PlatformShortName)/",
			"../external/gtest/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
			"../external/zlib/lib/"..target_env.."/$(PlatformShortName)-Release",
		}

	else
		if target_env == "macos" then
			-- OSX
			links
			{
				"SDL2.framework"
			}

			libdirs
			{
				"../external/libpng/lib/"..target_env.."/",
				"../external/zlib/lib/"..target_env.."/",
			}

			for _, p in ipairs(tbl_platforms) do
				for _, c in ipairs(tbl_configurations) do
					local pc = "/"..p.."-"..c
					filter{ "platforms:"..p, c }
					libdirs
					{
						"../external/tinyxml2/lib/"..target_env..pc,
						"../external/gtest/lib/"..target_env..pc,
					}
				end
			end
			filter {}

			-- note: unlike the libdirs, these are _two_ folders out. not entirely sure why.
			local framework_paths = {
				"-F../../external/SDL2/",
			}

			buildoptions(framework_paths)
			linkoptions(framework_paths)

		else
			-- other *nix
			links
			{
				"tinyxml2",
				"SDL2",
				"GL",
				"pthread"
			}
		end


		-- common *nix link options
		links
		{
			"png16",
			"z"
		}
	end

	-- post-build options
	if target_env == "windows" then -- copy SDL dlls
		local external_rel_path = "../../external/"; -- current directory at this point is .projects/windows
		local artifacts_rel_path = "../../"..bin_location.."/";
		for _, p in ipairs(tbl_platforms) do
			for _, c in ipairs(tbl_configurations) do
				local pc = p.."-"..c
				filter { "platforms:"..p, c }
				postbuildcommands{
					os.translateCommands("{COPY} "..external_rel_path.."SDL2/lib/"..target_env.."/"..p.."/*.dll "..artifacts_rel_path..pc),
				}
			end
		end
	end
