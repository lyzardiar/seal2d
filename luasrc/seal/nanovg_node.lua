local nanovg_core = require "nanovg_core"
local nanovg_node = {}

function nanovg_node.new(cmds)
    local self = {}
    setmetatable(self, {__index = nanovg_node})
    init(self, cmds)
    return self
end


return nanovg_node