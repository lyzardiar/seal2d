local lxp = require "lxp"
local platform = require "platform_core"
local score = {}

local function load(self, xml_path)
    local xml_data = platform.read_s(xml_path)
    local callbacks = {
        StartElement = function(_, tag, attr)
            printf("S: %s", tag)
            if attr and next(attr) then
                print_r(attr)
            end
        end,

        EndElement = function(_, tag, attr)
            printf("E: %s", tag)
            if attr and next(attr) then
                print_r(attr)
            end
        end,

        CharacterData = function(_, ...)
            print("CData:", ...)
        end
    }
    local p = lxp.new(callbacks)
    p:parse(xml_data)
    p:close()
end

function score.new(...)
    local self = {}
    setmetatable(self, {__index = score})

    load(self, ...)
    return self
end

function score:draw()
end

return score