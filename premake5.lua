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
	{ "android", "VS2015 projects targeting Android ARM and Intel, 32/64 bits" },
}

newoption
{
	trigger = "target",
	description = "Build environment and target to generate projects for.",
	allowed = tbl_target_values
}

-- validation
target_env = _OPTIONS["target"]
if not target_env then
	print "Command-line option --target is required with one of the following values:"
	for _, v in ipairs(tbl_target_values) do
		print(v[1])
	end
	os.exit(1)
end

-- create solution
workspace "xr3.SDL"

	sln_location = ".projects/"..target_env
	location(sln_location)
	
	if isVS() then
		startproject("unittests")
	end
	
	-- platforms setup
	tbl_platforms = {}
	if target_env == "windows" then
		table.insert(tbl_platforms, "x86")
		table.insert(tbl_platforms, "x64")
	elseif target_env == "android" then
		table.insert(tbl_platforms, "x86")
		table.insert(tbl_platforms, "x64")
		table.insert(tbl_platforms, "arm")
		table.insert(tbl_platforms, "arm64")
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
		optimize "Debug"
		flags {
			"Symbols"
		}
	filter{ "Release" }
		optimize "Full"
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
				local pc = p.."-"..c
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
	
	-- target-specific setup
	-- Windows
	filter {}
	if target_env == "windows" then
		system("windows")
		systemversion "8.1"
	end

	-- create projects
	filter {}
	include "xr3core/premake5.lua"
	
	filter {}
	include "xr3json/premake5.lua"
	
	filter {}
	include "xr3/premake5.lua"
	
	filter {}
	include "xr3ui/premake5.lua"
	
	-- VS unittests
	if isVS() then
		filter {}
		include "unittests/premake5.lua"
	end
	
	