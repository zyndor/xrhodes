--==============================================================================
--
-- XRhodes
--
-- copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
--
--==============================================================================
project "unittests"

	kind "ConsoleApp"
	
	files
	{
		"h/**.hpp",
		"src/**.cpp",
	}
	
	includedirs
	{
		"../external/gtest/include",
		"../external/libpng",
		"../unittests/h",
		"../xr3core/h",
		"../xr3json/h",
		"../xr3/h",
		"../xr3ui/h",
	}
	
	defines { "DATA_PATH=\""..path.getabsolute("./data").."\"" }
	
	links
	{
		"gtest",
		"gtest_main",
		"xr3core",
		"xr3json",
		"xr3.SDL",
		"xr3ui",
	}

	if isVS() then
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
			--"SDL2_mixer",
			
			"tinyxml2",
        }
    
        libdirs
        {
            "../external/libpng/lib/"..target_env.."/$(PlatformShortName)-Release",
            "../external/tinyxml2/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
            "../external/SDL2/lib/"..target_env.."/$(PlatformShortName)/",
            "../external/SDL2_mixer/lib/"..target_env.."/$(PlatformShortName)",
            "../external/gtest/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
            "../external/zlib/lib/"..target_env.."/$(PlatformShortName)-Release",
        }

    else
        if target_env == "macosx" then
            -- OSX
            links
            {
                "SDL2.framework"
				--"SDL2_mixer.framework"
            }
        
            -- note: unlike the libdirs, these are _two_ folders out. not entirely sure why.
            local framework_paths = {
                "-F../../external/SDL2/",
                "-F../../external/SDL2_mixer/",
            }
        
            buildoptions(framework_paths)
            linkoptions(framework_paths)
        else
            -- other *nix
            for _, p in ipairs(tbl_platforms) do
                for _, c in ipairs(tbl_configurations) do
                    local pc = "/"..p.."-"..c
                    filter{ "platforms:"..p, c }
                
                    libdirs
                    {
                        "../external/SDL2/lib/"..target_env..pc,
                        "../external/SDL2_image/lib/"..target_env..pc,
                        "../external/SDL2_mixer/lib/"..target_env..pc,
                    }
                end
            end
        end

        -- common *nix link options
        filter {}
        links
        {
            "png16",
            "z"
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
    end

    if target_env == "windows" then -- copy SDL dlls
		local external_rel_path = "../../external/"; -- current directory at this point is .projects/windows
		local artifacts_rel_path = "../../"..bin_location.."/";
		for _, p in ipairs(tbl_platforms) do
			for _, c in ipairs(tbl_configurations) do
				local pc = p.."-"..c
				filter { "platforms:"..p, c }
				postbuildcommands{
					os.translateCommands("{COPY} "..external_rel_path.."SDL2/lib/"..target_env.."/"..p.."/*.dll "..artifacts_rel_path..pc),
					os.translateCommands("{COPY} "..external_rel_path.."SDL2_mixer/lib/"..target_env.."/"..p.."/*.dll "..artifacts_rel_path..pc)
				}
			end
		end
	end