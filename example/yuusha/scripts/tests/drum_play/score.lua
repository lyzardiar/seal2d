local lxp = require "lxp"
local platform = require "platform_core"
local score = {}

local function load(self, xml_path)
    local xml_data = platform.read_s(xml_path)

    -- local callbacks = {
    --     StartElement = function(...)
    --         print("S: ", ...)
    --         -- if tag == 'defaults' then
    --         --     self.defaults = {}
    --         --     if tag == 'scaling' then
    --         --         self.defaults.scaling = self.defaults.scaling or {}
    --         --     end
    --         -- end

    --     end,

    --     EndElement = function(...)
    --         print("E: ", ...)
    --         -- printf("E: %s", tag)
    --         -- if attr and next(attr) then
    --             -- print_r(attr)
    --         -- end
    --     end,

    --     CharacterData = function(...)
    --         print("CData: ", ...)
    --         -- if tag == 'millimeters' then
    --             -- self.defaults.scaling.millimeters =
    --         -- end
    --     end
    -- }


    -- local p = lxp.new(callbacks)
    -- p:parse(xml_data)
    -- p:close()
    local lom = require "seal.thirdparty.lxp.lom"
    local s = lom.parse(xml_data)
    print(require("cjson").encode(s))

end

function score.new(...)
    local self = {}
    setmetatable(self, {__index = score})
    self.parts = {}

    load(self, ...)
    return self
end

function score:draw()
end

return score