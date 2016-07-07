local sprite_core = require "sprite_core"

local sprite_frame = require "seal.sprite_frame"
local sprite = {}

local all_sprites = {}

function sprite.new(atlas_name, frame_name) 
	local self = {}
	setmetatable(self, {__index = sprite})
	self:ctor(atlas_name, frame_name)
	all_sprites[self] = self
	return self
end

function sprite.new_container(...)
	local self = {}
	setmetatable(self, {__index = sprite})
	self.__cobj = sprite_core.new_container(...)
	return self
end

function sprite.new_clip(...)
	local self = {}
	setmetatable(self, {__index = sprite})
	self.__cobj = sprite_core.new_clip(...)
	return self
end

-- frame_name must be unique in all of the texture
function sprite:ctor(...)
	local __frame = sprite_frame.get(...)

	self.__cobj = sprite_core.new(__frame)
end

function sprite:register_handler(func)
	sprite_core.register_handler(self.__cobj, func)
end

function sprite:set_pos(x, y)
	sprite_core.set_pos(self.__cobj, x, y)
end

function sprite:set_scale(scale)
	sprite_core.set_scale(self.__cobj, scale)
end

function sprite:set_rotation(rotation)
	sprite_core.set_rotation(self.__cobj, rotation)
end

function sprite:add_child(child)
	sprite_core.add_child(self.__cobj, child.__cobj)
end

function sprite:remove_all_child()
	sprite_core.remove_all_child(self.__cobj)
end

function sprite:get_pos()
	return sprite_core.get_pos(self.__cobj)
end

function sprite:set_anim(frames)
	sprite_core.set_anim(self.__cobj, frames)
end

function sprite:set_anim_interval(interval)
	sprite_core.set_anim_interval(self.__cobj, interval)
end

return sprite