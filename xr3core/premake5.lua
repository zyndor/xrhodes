project "xr3core"

	kind "StaticLib"
	
	files {
		"h/**.hpp",
		"src/**.cpp",
	}
	
	includedirs {
		"h/",
	}