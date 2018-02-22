--==============================================================================
--
-- XRhodes
--
-- copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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
	{ "android", "VS2015 projects targeting Android ARM and Intel, 32/64 bits" },
    { "macosx", "Xcode4 projects targeting OS X" },
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
	elseif target_env == "macosx" then
		table.insert(tbl_platforms, "Universal64")
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
		
		desktop = true
	end

    -- Mac OSX
    if target_env == "macosx" then
		flags {
			"StaticRuntime"
		}
		
		desktop = true
    end

	if desktop then
		filter { "Debug" }
			defines  {
				"ENABLE_ASSET_BUILDING"
			}
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
	
	filter {}
	include "unittests/premake5.lua"
	
	