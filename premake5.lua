workspace "GameEngine"
    configurations { "Debug", "Release" }
    startproject "Billar"

    flags { "MultiProcessorCompile" }

    filter "configurations:Debug"
        defines { "DEBUG", "DEBUG_SHADER" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "Speed"
        flags { "LinkTimeOptimization" }

project "Billar"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
	architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    includedirs {
        "include/",
        "libs/glad/include/",
        "libs/glfw/include/",
        "libs/glm/",
        "libs/imgui/",
        "libs/imgui/examples",
        "libs/stb/include",
        "libs/assimp/include",
        "libs/bullet/src",
        "libs/irrKlang/include"
    }

    files { "src/**.cpp" }

    links { "GLFW", "GLM", "GLAD", "ImGui", "assimp", "bullet" }

    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

        links { "dl", "pthread" }   -- platform specific libraries

        links { "IrrKlang" }
        linkoptions {"-L./bin/%{cfg.buildcfg}/shared/ \'-Wl,-rpath,$$ORIGIN/shared/\'"}
        prebuildcommands {
            "mkdir bin/shared/ -p",
            "{COPY} libs/irrKlang/bin/linux-gcc-64/* bin/%{cfg.buildcfg}/shared/"
        }

        defines { "_X11" }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        
        defines { "_WINDOWS" }

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

include "libs/glfw.lua"
include "libs/glad.lua"
include "libs/glm.lua"
include "libs/imgui.lua"
include "libs/assimp.lua"
include "libs/bullet.lua"