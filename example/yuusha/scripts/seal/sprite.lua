local core = require "sprite.core"

local spriete_frame = require "sprite_frame"
local sprite = {}

local function gc(self)
	sprite_free(self.__cobj)
end

function sprite.new(...) 
	local self = {}
	setmetatable(self, {__index = sprite, __gc = gc})
	self:ctor(...)
	return self
end

-- frame_name must be unique in all of the texture
function sprite:ctor(frame_name, atlas_name)
	local frame = sprite_frame.get(frame_name, atlas_name)

	local __cobj = core.new(frame_name)

	self.__cobj = __cobj
end

function sprite:set_pos(x, y)
	sprite_set_pos(self.__cobj, x, y)
end

return sprite