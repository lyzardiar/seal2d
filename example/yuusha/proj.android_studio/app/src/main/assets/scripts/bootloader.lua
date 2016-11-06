__print = print
print = package.print

if inited == nil then 
    inited = false
end

require = function (name)
print(1)
    if package.loaded[name] then 
        return package.loaded[name]
    end
print(2)
    if false and package.preload[name] then
print(3)
        local md = package.preload[name]()
print(4)
        package.loaded[name] = md
print(5)
        return md
    else 
print(6)
        local func, err = package.lua_loader(name)
        if func then 
            local m = func()
            print(m, #m)
            for k, v in pairs(m) do 
                print(k, v)
            end
            return func()
        end
    end
end

local function hook()
    if inited then return end
    inited = true

    print(package.loaders, "======================================================================")
    
    package.loaders = package.loaders or {}
    print("->", #package.loaders)
    local preload = {}
    for k, v in pairs(package.preload) do 
        print(k, v)
        preload[k] = v
    end 
    print("-------------")
    for k, v in pairs(package.loaded) do 
        print(k, v)
    end 
    --platform_core = package.open_platform()
    for k, v in pairs(package) do 
        print(k, v)
    end 
    require "seal.util"
    require("platform_core")

    --local platform = require("platform_core")

    -- hook print, save the origin print to __print
    -- if platform.get_platform() == 'android' then
    --     __print = print
    --     print = platform.__print
    -- end

    -- local loaders = package.loaders or {}
    -- table.insert(loaders, platform.__loader)
    -- package.loaders = loaders

    -- uncomment this would trigger lplatform_lua_loader
    -- local not_exist = require "not_exist_file"
end

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
    elseif plat == 'android' then
        for _, path in ipairs(script_search_path) do
            package.path = package.path .. ";" .. path
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
        hook()

    inject("device", require "seal.device")
    inject("sprite", require "seal.sprite")
    inject("sprite_frame", require "seal.sprite_frame")

    local game = require "game"
    require("seal.engine").start(game)
end

--pmain()


local function test()
    hook()

    local function inject(mod_name, mod)
        for k, v in pairs(mod) do
            _G[k] = mod[k]
            print(string.format("inject: %s.%s", mod_name, k))
        end
        _G[mod_name] = mod
    end

    -- inject("util", require "seal.util")

    -- inject("device", require "seal.device")
    -- inject("sprite", require "seal.sprite")
    -- inject("sprite_frame", require "seal.sprite_frame")

    -- local game = require "game"
    -- require("seal.engine").start(game)
end

test()