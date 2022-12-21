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
        "libs/imgui/examples/",
        "libs/imgui/misc/cpp/",
        "libs/stb/include",
        "libs/assimp/include",
        "libs/bullet/src",
        "libs/irrKlang/include"
    }

    files { "src/**.cpp", "include/**.h", "include/**.hpp" }

    links { "GLFW", "GLM", "GLAD", "ImGui", "assimp", "bullet" }
    openmp "On"
    defines {
        "B3_USE_CLEW",
        "BT_THREADSAFE",
		"BT_USE_OPENMP"
    }

    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

        links { "dl", "pthread" }   -- platform specific libraries

        links { "IrrKlang" }        -- librerias dinamicas
        linkoptions {"-fopenmp -L./bin/%{cfg.buildcfg}/ \'-Wl,-rpath,$$ORIGIN\'"}    -- esto es para definir la carpeta donde se encuentra el .so y que en runtime coja el .so de la misma carpeta en la que este el ejecutable
        prebuildcommands {
            "mkdir bin/%{cfg.buildcfg}/ -p",
            "{COPY} libs/irrKlang/bin/linux-gcc-64/* bin/%{cfg.buildcfg}"   -- librerias .so que se usaran con el ejecutable (si le pasamos el juego a alguien hay que darle tambien los .so)
        }

        defines { "_X11" }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"

        links { "irrKlang" }                        -- librerias dinamicas
        libdirs { "bin/%{cfg.buildcfg}" }
        -- linkoptions {"-L./bin/%{cfg.buildcfg}"}     -- define la carpeta donde se encuentran las librerias dinamicas

        prebuildcommands {  -- mover archivos de librerias dinamicas a la carpeta de compilacion
            "mkdir bin/%{cfg.buildcfg}/ -p",
            "{COPY} libs/irrKlang/lib/Winx64-visualStudio/* bin/%{cfg.buildcfg}/",    -- librerias .lib que solo se van a usar para el compilado
            "{COPY} libs/irrKlang/bin/winx64-visualStudio/*.dll bin/%{cfg.buildcfg}/" -- librerias .dll que se usaran con el .exe (si le pasamos el juego a alguien hay que darle tambien los .dll)
        }

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