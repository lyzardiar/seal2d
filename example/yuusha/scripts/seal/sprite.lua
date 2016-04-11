local sprite_core = require "sprite_core"

local sprite_frame = require "seal.sprite_frame"
local sprite = {}

local function gc(self)
	print("release sprite")
	sprite_core.free(self.__cobj)
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

	local __cobj = sprite_core.new(frame.__cobj)

	self.__cobj = __cobj
end

function sprite:set_pos(x, y)
	sprite_set_pos(self.__cobj, x, y)
end

function sprite:add_child(child)
	sprite_add_child(self.__cobj, child.__cobj)
end

return sprite