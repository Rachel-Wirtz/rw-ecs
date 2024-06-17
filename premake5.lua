workspace "rw-ecs"	
    cppdialect      "C++20"
    cdialect        "C17"
    characterset    "MBCS"
    staticruntime   "Off"
    targetdir       "%{wks.location}/output/bin/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/"
    objdir          "%{wks.location}/output/obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/"
	startproject	"rw-ecs-demo"
	
	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	platforms {
		"x86",
		"x64"
	}
			
	filter "configurations:Debug"
		runtime 	"Debug"
		symbols 	"On"
		
	filter "configurations:Release"
		runtime		"Release"
		optimize	"speed"
		
	filter "configurations:Dist"
		runtime		"Release"
		optimize	"speed"
		symbols		"Off"
		
	project "rw-ecs"
		kind			"StaticLib"
		location		"rw-ecs"

		files {			
			"rw-ecs/include/**.h",
			"rw-ecs/include/**.hpp",
			"rw-ecs/source/**.cpp"
		}
	
		includedirs {
			"rw-ecs/include/"
		}        

	project "rw-ecs-demo"
        kind            "ConsoleApp"
        location        "demo"
        language        "C++"
		
		files {
			"demo/**.h",
			"demo/**.hpp",
			"demo/**.cpp"
		}
	
		includedirs {
			"rw-ecs/include/"
		}
        
		links {
			"rw-ecs"
		}