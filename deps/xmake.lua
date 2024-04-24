includes("RE-UE4SS")

target("apclientpp")
    set_kind("headeronly")
    add_includedirs(path.join(os.scriptdir(), "apclientpp"), { public = true })
target_end()

target("asio")
    set_kind("headeronly")
    add_includedirs(path.join(os.scriptdir(), "asio/asio/include"), { public = true })
target_end()

package("json")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "json"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("json")

package("valijson")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "valijson"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("valijson")

package("websocketpp")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "websocketpp"))

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("websocketpp")

target("wswrap")
    set_kind("headeronly")
    add_includedirs(path.join(os.scriptdir(), "wswrap/include"), { public = true })

