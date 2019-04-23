--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
--
-- Prints the contents of a table 't'.
--
function debug_print_table(t)
	for _, v in ipairs(t) do
		print(v)
	end
end

--
-- Determines whether the action given to premake5 was for Visual Studio.
--
function is_vs()
	return _ACTION ~= nil and _ACTION:find("vs") == 1
end

--
-- Performs the creation of a workspace with the given name, and making the
-- given project its default project (Visual Studio only).
--
function do_workspace(workspace_name, start_project_name)
	-- target option
	tbl_target_values =
	{
		{ "windows", "VS2015 projects targeting Windows 32/64 bits" },
		{ "macos", "Xcode4 projects targeting OS X" },
		{ "linux", "Makefile projects targeting Linux (64 bits)" },
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
	workspace(workspace_name)

	sln_location = ".projects/"..target_env
	location(sln_location)

	if is_vs() then
		startproject(start_project_name)
	end

	-- platforms setup
	tbl_platforms = {}
	if target_env == "windows" then
		table.insert(tbl_platforms, "x86")
		table.insert(tbl_platforms, "x64")
	elseif target_env == "linux" then
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

	if is_vs() then
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

	if is_vs() == false then
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
		if is_vs() then
			flags { "MultiProcessorCompile" }
			
			buildoptions {
				"/WX",
				--"/Wall", -- Stricter checks would be great; Visual Studio going crazy with warnings, less so. Most of them are not related or are related to safe casts etc.
			}
		end

		system("windows")
		systemversion "8.1"

		target_desktop = true
	end

	-- Mac OS
	if target_env == "macos" then
		staticruntime "On"

		target_desktop = true
	end

	-- Linux
	if target_env == "linux" then
		target_desktop = true
	end

	-- all desktop builds: enable asset building in debug
	if target_desktop then
		filter { "Debug" }
			defines  {
				"ENABLE_ASSET_BUILDING"
			}
	end

end