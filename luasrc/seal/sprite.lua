local sprite_core = require "sprite_core"

local sprite_frame = require "seal.sprite_frame"
local sprite = {}

local meta = {
	__index = function(t, key, ...)
		local core_filed = sprite_core[key]
		if core_filed then
			if type(core_filed) == 'function' then
				local handle = function(t, ...)
					return core_filed(t.__cobj, ...)
				end
				rawset(t, key, handle)
				return handle
			end
		else
			return rawget(sprite, key)
		end
	end
}

local all_sprites = {}

function sprite.new(...)
	local self = {}
	setmetatable(self, meta)
	local __frame = sprite_frame.get(...)
	self.__cobj = sprite_core.new(__frame)

	all_sprites[self] = self
	return self
end

function sprite.new_label(...)
	local self = {}
	setmetatable(self, meta)
	self.__cobj = sprite_core.new_label(...)

	return self
end

function sprite.new_bmfont_label(...)
	local self = {}
	setmetatable(self, meta)
	self.__cobj = sprite_core.new_bmfont_label(...)
	return self
end

function sprite.new_container(...)
	local self = {}
	setmetatable(self, meta)
	self.__cobj = sprite_core.new_container(...)
	return self
end

function sprite.new_primitive(...)
	local self = {}
	setmetatable(self, meta)
	self.__cobj = sprite_core.new_primitive(...)
	return self
end

function sprite.new_clip(...)
	local self = {}
	setmetatable(self, meta)
	self.__cobj = sprite_core.new_clip(...)
	return self
end

function sprite:cleanup(...)
	self:remove_all_child(...)
	self:clean_handler()
end

return sprite