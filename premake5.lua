--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
-- helpers
function debug_print_table(t)
	for _, v in ipairs(t) do
		print(v)
	end
end

function isVS()
	return _ACTION ~= nil and _ACTION:find("vs") == 1
end

-- target option
tbl_target_values =
{
	{ "windows", "VS2015 projects targeting Windows 32/64 bits" },
    { "macos", "Xcode4 projects targeting OS X" },
}

newoption
{
	trigger = "target",
	description = "Build environment and target to generate projects for.",
	allowed = tbl_target_values
}

-- validate target
target_env = _OPTIONS["target"]
if not target_env then
	print "Command-line option --target is required with one of the following values:"
	for _, v in ipairs(tbl_target_values) do
		print(v[1])
	end
	os.exit(1)
end

-- create solution
workspace "xr3"

	sln_location = ".projects/"..target_env
	location(sln_location)

	if isVS() then
		startproject("examples")
	end

	-- platforms setup
	tbl_platforms = {}
	if target_env == "windows" then
		table.insert(tbl_platforms, "x86")
		table.insert(tbl_platforms, "x64")
	elseif target_env == "macos" then
		table.insert(tbl_platforms, "Universal64")
	end
	platforms(tbl_platforms)

	-- configurations
	tbl_configurations = {
		"Debug",
		"Release"
	}
	configurations(tbl_configurations)

	-- shared project settings
	language "C++"
	editandcontinue("Off")
	floatingpoint("Fast")
	rtti("Off")
	characterset("MBCS")

	-- optimization
	filter("not Release")
		optimize "Off"
		symbols "On"
		runtime "Debug"
	filter{ "Release" }
		optimize "Full"
		runtime "Release"
	filter{}

	-- paths
	debugdir ""

	bin_location = ".artifacts/"..target_env
	obj_location = ".intermediate/"..target_env

	if isVS() then
		local pc = "/$(PlatformShortName)-$(Configuration)"
		targetdir(bin_location..pc)
		libdirs(bin_location..pc)
		objdir(obj_location..pc)
	else
		for _, p in ipairs(tbl_platforms) do
			for _, c in ipairs(tbl_configurations) do
				local pc = "/"..p.."-"..c
				filter{ "platforms:"..p, c }
					targetdir(bin_location..pc)
					libdirs(bin_location..pc)
					objdir(obj_location..pc)
			end
		end
	end

	-- defines
	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"_SCL_SECURE_NO_WARNINGS",
	}

	filter{ "Debug" }
		defines {
			"XR_DEBUG"
		}
	filter{}

	if isVS() == false then
		buildoptions {
			"-std=c++14",
			"-Wall",
			"-Werror",
			"-fvisibility-ms-compat",
		}
	end

	-- target-specific setup
	filter {}
	-- Windows
	if target_env == "windows" then
		if isVS() then
			buildoptions {
				"/WX",
				--"/Wall", -- Stricter checks would be great; Visual Studio going crazy with warnings, less so. Most of them are not related or are related to safe casts etc.
			}
		end

		system("windows")
		systemversion "8.1"

		target_desktop = true
	end

    -- Mac OSX
    if target_env == "macos" then
		flags {
			"StaticRuntime"
		}

		target_desktop = true
    end

	-- all desktop builds: enable asset building in debug
	if target_desktop then
		filter { "Debug" }
			defines  {
				"ENABLE_ASSET_BUILDING"
			}
	end

	-- create a project that just reruns the premake script.
	project "!regenerate_projects"
	kind "ConsoleApp"	-- not actually, but it has to be something.
	local generate_projects_suffix = "";
	if target_env == "windows" then
		generate_projects_suffix = ".bat"
	else
		generate_projects_suffix = ".sh"
	end

	postbuildcommands{
		-- at this point we're in .projects/${target_env}/
		"../../tools/"..target_env.."/generate_projects"..generate_projects_suffix,
	}
	filter {}

	-- create projects
	filter {}
	include "xr3core/premake5.lua"

	filter {}
	include "xr3json/premake5.lua"

	filter {}
	include "xr3/premake5.lua"

	filter {}
	include "xr3scene/premake5.lua"

	filter {}
	include "unittests/premake5.lua"

	filter {}
	include "examples/premake5.lua"

