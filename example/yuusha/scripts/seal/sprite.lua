local core = require "sprite.core"

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

--[[
	Data Structure Design

	SpriteFrame -> {
		name,
		texture_file_name,
		tex_id,	 			->check valid before we use the texture. 
		x, y,
		width, height,
	}

	sprite frame only contains data.

	Texture -> {
		tex_id, -> this is the id created by the OpenGL

		width, height,
		file_name,
	}
	
	every sprite requires a frame to create, make it simple
	Sprite -> {
	
	}
]]

-- frame_name must be unique in all of the texture
function sprite:ctor(frame_name)
	
	local __cobj = core.new(frame_name)

	self.__cobj = __cobj
end

function sprite:set_pos(x, y)
	sprite_set_pos(self.__cobj, x, y)
end

return sprite