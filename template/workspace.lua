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

function is_clang()
	return _ACTION ~= nil and (_ACTION:find("xcode") == 1 or _ACTION:find("clang"))
end

function is_msvc()
	return is_vs() and not _ACTION:find("clang")
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

	filter{ "Debug" }
		defines {
			"XR_DEBUG"
		}

	filter{}

	if is_msvc() then
		-- defines
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_SCL_SECURE_NO_WARNINGS",
		}

		buildoptions {
			"/std:c++17",
			"/permissive-",
			"/W4",
			"/we4244", -- conversion from 'type1' to 'type2', possible loss of data
			"/we4263", -- 'function': member function does not override any base class virtual member function.
			"/we4265", -- virtual methods and no virtual dtor.
			"/we4289", -- nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
			"/we4296", -- 'operator': expression is always 'boolean_value'
			"/we4545", -- expression before comma evaluates to function call, missing argument list;
			"/we4546", -- function call before comma missing argument list;
			"/we4547", -- operator: operator before comma has no effect, expected operator with side-effect;
			"/we4549", -- operator: operator before comma missing argument list;
			"/we4555", -- expression has no effect; expected: expression with side-effect;
			"/w14619", -- pragma warning: there is no warning number 'number';
			"/we4640", -- Enable warning on thread un-safe static member initialization
			"/we4826", -- Coversion from 'type1' to 'type2' is sign-extended. This may cause unexpected runtime behavior;
			"/we4905", -- wide string literal cast to 'LPSTR'
			"/we4906", -- string literal cast to 'LPWSTR'
			"/we4928", -- illegal copy-initialization; more than one user-defined conversion has been implicitly applied
			--"/Wall", -- Stricter checks would be great; Visual Studio going crazy with warnings, less so. Most of them are not related or are related to safe casts etc.
		}
	else
		buildoptions {
			"-std=c++17",
			"-pedantic",
			"-Wall",
			"-Wdouble-promotion",
			"-Werror",
			"-Wextra",
			"-Wmissing-declarations",
			"-Wnon-virtual-dtor",
			"-Wold-style-cast",
			"-Wshadow",
			"-fvisibility-ms-compat",
		}

		if not is_clang() then -- probably GCC
			buildoptions {
				"-Wduplicated-branches",
				"-Wduplicated-cond",
				"-Wlogical-op",
				"-Wmisleading-indentation",
			}
		end
	end

	-- target-specific setup
	filter {}

	-- Windows
	if target_env == "windows" then
		if is_vs() then
			flags { "MultiProcessorCompile" }
		end

		system("windows")
		systemversion "8.1"
		defines {
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX",
		}

		target_desktop = true
	end

	-- Mac OS
	if target_env == "macos" then
		includedirs
		{
			"/usr/local/include"
		}

		libdirs
		{
			"/usr/local/lib"
		}

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

--
-- Copies dependencies on Windows
--
function do_vs_postbuild(dependencies)
	local artifacts_rel_path = "../../"..bin_location.."/"; -- we're currently in .projects/${platform}/
	for _, p in ipairs(tbl_platforms) do
		for _, c in ipairs(tbl_configurations) do
			filter { "platforms:"..p, c }
				local pc = p.."-"..c
				for _, iDep in ipairs(dependencies) do
					postbuildcommands{
						os.translateCommands("{COPY} $(VcpkgCurrentInstalledDir)"..
							"bin/"..iDep..".dll "..artifacts_rel_path..pc)
					}
				end
			filter {}
		end
	end
end
