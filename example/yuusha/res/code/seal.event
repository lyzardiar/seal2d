local sdl_enum = require "seal.sdl_enum"
local event = {}

local MOUSE_EVENTS = {}

local touch_event = {}

function touch_event.new(type, x, y, dx, dy)
	local obj = {
		type = type,
		x = x,
		y = y,
		dx = dx, 
		dy = dy,
		captered = {},
		current_target = nil,
		stop = false
	}
	setmetatable(obj, {__index = touch_event} )
	return obj
end

function touch_event:add_captured(sprite)
	self.captered[#self.captered+1] = sprite
end

function touch_event:stop_bubble()
	self.stop = true
end

local tmp_event
local function on_touch(type, x, y, dx, dy, root)
	if type == 'begin' then
		tmp_event = touch_event.new(type, x, WINDOW_HEIGHT - y, dx, dy)
		root:touch_test(tmp_event)
		return
	end

	if tmp_event then
		tmp_event.type = type
		root:touch_test(tmp_event)

		if type == 'end' then
			tmp_event = nil
		end
	end
end

MOUSE_EVENTS[sdl_enum.SDL_MOUSEBUTTONDOWN] = function(e, ...)
	on_touch('begin', e.x, e.y, 0, 0, ...)
end

MOUSE_EVENTS[sdl_enum.SDL_MOUSEMOTION] = function(e, ...)
	on_touch('move', e.x, e.y, e.xrel, e.yrel, ...)
end

MOUSE_EVENTS[sdl_enum.SDL_MOUSEBUTTONUP] = function(e, ...)
	on_touch('end', e.x, e.y, 0, 0, ...)
end

MOUSE_EVENTS[sdl_enum.SDL_MOUSEWHEEL] = function(e, ...)

end

MOUSE_EVENTS[sdl_enum.SDL_FINGERDOWN] = function(e, ...)
	-- on_touch('begin' e.x, e.y, e.dx, e.dy, ...)
end

MOUSE_EVENTS[sdl_enum.SDL_FINGERMOTION] = function(e, ...)
	-- on_touch(e.x, e.y, e.dx, e.dy, ...)
end

MOUSE_EVENTS[sdl_enum.SDL_FINGERUP] = function(e, ...)
	-- on_touch(e.x, e.y, e.dx, e.dy, ...)
end

MOUSE_EVENTS[sdl_enum.SDL_DOLLARGESTURE] = function(e, ...)
	print_r(e)
end
MOUSE_EVENTS[sdl_enum.SDL_DOLLARRECORD] = function(e, ...)
	print_r(e)
end
MOUSE_EVENTS[sdl_enum.SDL_MULTIGESTURE] = function(e, ...)
	print_r(e)
end

function event.mouse(e, root)
	local f = MOUSE_EVENTS[e.type]

	if not f then
		print('no event for ', e.type)
	else
		f(e, root)
	end
end

return event