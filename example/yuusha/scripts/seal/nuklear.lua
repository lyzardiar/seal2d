local nuk_core = require "nuklear_core"

local nuklear = {
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

function nuklear.init()
	return setmetatable(nuklear, {__index = function(t, key, ...)
			print(string.format("call metatable nuk_core[%s] = %s", key, nuk_core[key]))
			rawset(t, key, nuk_core[key])
			return nuk_core[key]
		end})
end

function nuklear.new_panel()

end

return nuklear