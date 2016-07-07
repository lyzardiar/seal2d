local nuk_core = require "nuklear_core"
local nuk_node = {}

function nuk_node.register(func)
    nuk_core.nuk_node_register(func)
end

return nuk_node