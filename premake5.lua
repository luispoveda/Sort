workspace "Sort"
    platforms
    {
        "Win32",
        "Win64"
    }

    configurations
    {
        "Debug",
        "Release"
    }

    filter "platforms:Win32"
        architecture "x86"

    filter "platforms:Win64"
        architecture "x86_64"

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

project "Sort"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    targetdir ("bin/" .. outputdir)
    objdir ("build/" .. outputdir)

    warnings "Extra"

    files
    {
        "include/*.hpp",
        "test/*.hpp",
        "test/*.cpp"
    }

    includedirs
    {
        "include"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "Speed"
