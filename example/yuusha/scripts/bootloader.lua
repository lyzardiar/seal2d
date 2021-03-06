local function pmain()
    local platform = require("platform_core")
    local script_search_path = {
        "?.lua",
        "scripts/?.lua",
        "seal/?.lua",
        "thirdparty/socket/?.lua",
        "thirdparty/cov/?.lua;",
        "thirdparty/luaunit/?.lua;",
        "res/?.lua",
    }

    local plat = platform.get_platform()
    if plat == 'mac' then
        for _, path in ipairs(script_search_path) do
            package.path = package.path .. ";" .. path
        end

    elseif plat == 'ios' then
        local root = platform.get_sandbox_root_path()
        print("ios root path is ", root)
        for _, path in ipairs(script_search_path) do
            package.path = package.path .. ";" .. root .. "/" .. path
        end
    elseif plat == 'win' then
        local script_search_path = {
            "?.lua",
            "../../scripts/?.lua",
            "../../../../luasrc/?.lua",
            "../../../../luasrc/seal/?.lua",
            "../../../../luasrc/thirdparty/socket/?.lua",
            "../../../../luasrc/thirdparty/cov/?.lua;",
            "../../../../luasrc/thirdparty/luaunit/?.lua;",
            "../../res/?.lua",
        }
        local root = platform.get_sandbox_root_path()
        print("win root path is ", root)
        for _, path in ipairs(script_search_path) do
            package.path = package.path .. ";" .. root .. path
        end
    else
        assert(false, "other platform should write the load function.")
    end

    -- print("package.path = ", package.path)
    -- require "luacov"
    -- require "luacov.tick"
    local function inject(mod_name, mod)
        for k, v in pairs(mod) do
            _G[k] = mod[k]
            print(string.format("inject: %s.%s", mod_name, k))
        end
        _G[mod_name] = mod
    end

    inject("util", require "seal.util")
    inject("device", require "seal.device")
    inject("sprite", require "seal.sprite")
    inject("sprite_frame", require "seal.sprite_frame")

    -- hook print, save the origin print to __print
    __print = print
    print = require("platform_core").__print

    local game = require "game"
    require("seal.engine").start(game)
end

function main()
    xpcall(pmain, function(err)
            print(err)
            print(debug.traceback("", 2))
        end)
end