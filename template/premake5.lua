--
-- XRhodes based project template
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
-- Requires:
-- * The location of xr3 added to the environment as XR3_PATH;
-- * A 'config.lua' file in the same directory as this file;

dofile("config.lua")

xr3_path = os.getenv("XR3_PATH")
if xr3_path == "" then
	print "XR3_PATH is required to be set in your environment."
	os.exit(1)
else
	local slashes = { '/', '\\' }
	if slashes[ string.sub(xr3_path, -1) ] == nil then
		-- append slash if missing from path
		xr3_path = xr3_path.."/"
	end
end

include("workspace.lua")

do_workspace(config.projectName, config.projectName)

--
-- create a project that just reruns the premake script.
--
filter {}
project "!regenerate_projects"
kind "Utility"
local generate_projects_suffix = "";
if target_env == "windows" then
	generate_projects_suffix = ".bat"
else
	generate_projects_suffix = ".sh"
end

postbuildcommands{
	-- at this point we're in .projects/${target_env}/
	"../../generate_projects"..generate_projects_suffix,
}

--
-- main project
--
filter {}
project(config.projectName)

kind "ConsoleApp"

files(config.files)

includedirs
{
	xr3_path.."external/libpng",
	xr3_path.."unittests/h",
	xr3_path.."xr3core/h",
	xr3_path.."xr3json/h",
	xr3_path.."xr3/h",
	xr3_path.."xr3scene/h",
}
if config.includeDirs ~= nil then
	includedirs(config.includeDirs)
end

defines { "DATA_PATH=\""..path.getabsolute("./data").."\"" }

links
{
	"xr3scene",
	"xr3",
	"xr3json",
	"xr3core",
}

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
		xr3_path..".artifacts/"..target_env.."/$(PlatformShortName)-$(Configuration)",
		xr3_path.."external/libpng/lib/"..target_env.."/$(PlatformShortName)-Release",
		xr3_path.."external/tinyxml2/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
		xr3_path.."external/SDL2/lib/"..target_env.."/$(PlatformShortName)/",
		xr3_path.."external/zlib/lib/"..target_env.."/$(PlatformShortName)-Release",
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
			xr3_path.."external/libpng/lib/"..target_env.."/",
			xr3_path.."external/zlib/lib/"..target_env.."/",
		}

		for _, p in ipairs(tbl_platforms) do
			for _, c in ipairs(tbl_configurations) do
				local pc = "/"..p.."-"..c
				filter{ "platforms:"..p, c }
				libdirs
				{
					xr3_path.."external/tinyxml2/lib/"..target_env..pc,
				}
			end
		end
		filter {}

		local framework_paths = {
			xr3_path.."external/SDL2/",
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

if target_env == "windows" then -- copy SDL dlls
	local external_rel_path = xr3_path.."external/"; -- current directory at this point is .projects/windows
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
