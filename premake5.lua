workspace "rw-ecs"	
    cppdialect      "C++20"
    cdialect        "C17"
    characterset    "MBCS"
    staticruntime   "Off"
    targetdir       "%{wks.location}/output/bin/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/"
    objdir          "%{wks.location}/output/obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/"
	
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
		kind			"SharedItems"
		location		"include"

		files {			
			"include/**.h",
			"include/**.hpp"
		}

	project "rw-ecs-demo"
        kind            "ConsoleApp"
        location        "demo"
        language        "C++"
		
		files {
			"rw-ecs/demo/**.h",
			"rw-ecs/demo/**.hpp",
			"rw-ecs/demo/**.c",
			"rw-ecs/demo/**.cpp"
		}
	
		includedirs {
			"%{wks.location}/include/"
		}
        
		links {
			"rw-ecs"
		}