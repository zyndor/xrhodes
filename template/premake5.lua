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

-- link options
libdirs
{
	xr3_path..".artifacts/"..target_env.."/$(PlatformShortName)-$(Configuration)",
}

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
			"GL"	
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
