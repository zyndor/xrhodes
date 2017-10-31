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

    if target_env == "windows" then
        links
        {
            "libpng16",
            "zlib",
            "opengl32",
        }
    
        libdirs
        {
            "../external/libpng/lib/"..target_env.."/$(PlatformShortName)-Release",
            "../external/tinyxml/lib/$(Platform)/Release",
            "../external/SDL2/lib/$(PlatformShortName)/",
            "../external/SDL2_mixer/lib/$(PlatformShortName)",
            "../external/gtest/lib/"..target_env.."/$(PlatformShortName)-$(Configuration)",
            "../external/zlib/lib/"..target_env.."/$(PlatformShortName)-Release",
        }

    else
        if target_env == "macosx" then
            -- note: unlike the libdirs, these are _two_ folders out. not entirely sure why.
            local framework_paths = {
                "-F../../external/SDL2/",
                "-F../../external/SDL2_mixer/",
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
                        "../external/SDL2/lib/"..target_env..pc,
                        "../external/SDL2_image/lib/"..target_env..pc,
                        "../external/SDL2_mixer/lib/"..target_env..pc,
                    }
                end
            end
        end

        -- common *nix libdirs
        filter {}
        links
        {
            "png16",
            "z"
        }

        libdirs
        {
            "../external/libpng/lib/"..target_env.."/",
            "../external/tinyxml/lib/"..target_env.."/",
            "../external/zlib/lib/"..target_env.."/",
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
