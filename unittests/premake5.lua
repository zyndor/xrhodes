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

    if target_env == "windows" then
        -- TODO: links { SDL & friends }
    
        libdirs
        {
            "../external/tinyxml-2.6.2/lib/$(Platform)/Release",
            "../external/SDL2-2.0.4/lib/$(PlatformShortName)/",
            "../external/SDL2_image-2.0.1/lib/$(PlatformShortName)",
            "../external/SDL2_mixer-2.0.1/lib/$(PlatformShortName)",
            "../external/glew-2.0.0/lib/Release/$(PlatformShortName)",
            "../external/gtest/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
        }

    else
        if target_env == "macosx" then
            -- note: unlike the libdirs, these are _two_ folders out. not entirely sure why.
            local framework_paths = {
                "-F../../external/SDL2-2.0.4/",
                "-F../../external/SDL2_image-2.0.1/",
                "-F../../external/SDL2_mixer-2.0.1/",
            }
        
            buildoptions(framework_paths)
            linkoptions(framework_paths)
        else
            for _, p in ipairs(tbl_platforms) do
                for _, c in ipairs(tbl_configurations) do
                    local pc = "/"..p.."-"..c
                    filter{ "platforms:"..p, c }
                
                    libdirs
                    {
                        "../external/SDL2-2.0.4/lib/"..target_env..pc,
                        "../external/SDL2_image-2.0.1/lib/"..target_env..pc,
                        "../external/SDL2_mixer-2.0.1/lib/"..target_env..pc,
                        --"../external/glew-2.0.0/lib/"..target_env.."/",
                    }
                end
            end
        end

        -- common *nix libdirs
        filter {}
        libdirs
        {
            "../external/tinyxml-2.6.2/lib/"..target_env.."/",
        }
        
		for _, p in ipairs(tbl_platforms) do
			for _, c in ipairs(tbl_configurations) do
				local pc = "/"..p.."-"..c
				filter{ "platforms:"..p, c }
    
                libdirs
                {
                    "../external/gtest/lib/"..target_env..pc,
                }
            end
        end
    end
