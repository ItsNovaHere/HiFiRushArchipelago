local projectName = "hibiki"

target(projectName)
    set_kind("shared")
    set_languages("cxx23")
    set_exceptions("cxx")

    add_includedirs(".")
    add_files("*.cpp")
    add_deps("UE4SS")
    add_deps("apclientpp")
    add_deps("asio")
    add_packages("json")
    add_deps("valijson")
    add_packages("websocketpp")
    add_deps("wswrap")

    add_defines("ASIO_STANDALONE")
    add_defines("ASIO_NO_WIN32_LEAN_AND_MEAN")
    add_defines("WIN32_LEAN_AND_MEAN")
    add_defines("_WIN32_WINNT=0x0600")
    add_defines("WSWRAP_NO_SSL") -- disable ssl for now
    add_defines("_WEBSOCKETPP_CPP11_INTERNAL_")
    add_defines("NOMINMAX")

    on_load(function (target)
        import("build_configs", { rootdir = get_config("scriptsRoot") })
        build_configs:set_output_dir(target)
    end)

    on_config(function (target)
        import("build_configs", { rootdir = get_config("scriptsRoot") })
        build_configs:config(target)
    end)

    after_clean(function (target)
        import("build_configs", { rootdir = get_config("scriptsRoot") })
        build_configs:clean_output_dir(target)
    end)

    after_link(function (target)
           os.cp(target:targetfile(), 'D:\\Steam\\steamapps\\common\\Hi-Fi RUSH\\Hibiki\\Binaries\\Win64\\Mods\\hibiki\\dlls\\main.dll')
    end)