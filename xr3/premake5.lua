project "xr3.SDL"

	kind "StaticLib"
	
	files {
		"h/**.hpp",
		"src/*.cpp",
		"src/SDL/*.hpp",
		"src/SDL/*.cpp",
	}
	
	includedirs {
		"h/",
		"../xr3core/h/",
		"../xr3json/h/",
		"../external/glew-2.0.0/include/",
		"../external/SDL2-2.0.4/include/",
		"../external/SDL2_image-2.0.1/include",
		"../external/SDL2_mixer-2.0.1/include",
		"../external/tinyxml-2.6.2",
	}