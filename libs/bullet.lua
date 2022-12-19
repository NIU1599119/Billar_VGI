project "bullet"
	kind "StaticLib"
	language "C"
	architecture "x86_64"

	targetdir "../bin/%{cfg.buildcfg}"
	objdir "../obj/%{cfg.buildcfg}"
	
	includedirs { "bullet/src/" }

	files
	{
        "bullet/src/*",
		"bullet/src/LinearMath/**",
		"bullet/src/BulletCollision/**",
		"bullet/src/BulletDynamics/**",
		"bullet/src/BulletInverseDynamics/**",
		"bullet/src/BulletSoftBody/**",
        "bullet/src/Bullet3Common/**",
		"bullet/src/Bullet3Collision/**",
		"bullet/src/Bullet3Geometry/**",
		"bullet/src/Bullet3OpenCL/**",
		"bullet/src/Bullet3Serialize/**",
		"bullet/src/clew/**",
	}

	defines {
		"B3_USE_CLEW",
	}
    
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		defines
		{
			"_BULLET_X11"
		}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		defines 
		{
			"BT_USE_SSE_IN_API",	-- this is disabled in windows by default
									-- adds a bit of performance gain if used correctly
									-- library says its harder to integrate under windows with this
									-- library does not compile unless this is enabled :D
			"_BULLET_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
