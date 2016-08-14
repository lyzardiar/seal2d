local nuk_core = require "nuklear_core"

local nuk_node = {
	NK_WINDOW_BORDER        = 1 << 0,
	NK_WINDOW_BORDER_HEADER = 1 << 1,
	NK_WINDOW_MOVABLE       = 1 << 2,
	NK_WINDOW_SCALABLE      = 1 << 3,
	NK_WINDOW_CLOSABLE      = 1 << 4,
	NK_WINDOW_MINIMIZABLE   = 1 << 5,
	NK_WINDOW_DYNAMIC       = 1 << 6,
	NK_WINDOW_NO_SCROLLBAR  = 1 << 7,
	NK_WINDOW_TITLE         = 1 << 8,

	NK_BUTTON_DEFAULT = 0,
	NK_BUTTON_REPEATER = 1,
}

local meta = {
	__index = function(t, key, ...)
		local lua_func = rawget(nuk_node, key)
		if lua_func then
			return lua_func
		else
			local core_filed = nuk_core[key]
			if core_filed then
				rawset(t, key, core_filed)
				return core_filed
			end
		end
	end
}

function nuk_node.new()
	local obj = {
		__panel = nuk_core.nuk_new_panel()
	}
	setmetatable(obj, meta)

	return obj
end

function nuk_node:nk_begin(title, rect, flags)
	local flags = flags or (nuk_node.NK_WINDOW_BORDER      |
							nuk_node.NK_WINDOW_MOVABLE     |
							nuk_node.NK_WINDOW_SCALABLE    |
							nuk_node.NK_WINDOW_MINIMIZABLE |
							nuk_node.NK_WINDOW_TITLE)
	return nuk_core.nk_begin(title, self.__panel, rect, flags)
end

return nuk_node