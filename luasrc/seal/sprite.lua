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

function sprite.new(...)
	local self = {}
	setmetatable(self, meta)
	local __frame = sprite_frame.get(...)
	self.__cobj = sprite_core.new(__frame)

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

function sprite.new_spine(atlas_file, spine_data_file, scale)
	local self = {}
	setmetatable(self, meta)
	self.__cobj = sprite_core.new_spine(atlas_file, spine_data_file, scale or 1.0)
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

function sprite:set_bbox_visible(visible)
	if not self.__bbox_lines then
		local g = self:get_glyph()

		local lines = {
			{g[1], g[2], g[3], g[4]},
			{g[3], g[4], g[5], g[6]},
			{g[5], g[6], g[7], g[8]},
			{g[7], g[8], g[1], g[2]},
		}
		local bbox_lines = {}
		for i = 1, #lines do
			local line = sprite.new_primitive("L", {
											vertex = lines[i],
											width = 2.0,
											color = {30, 0, 0, 255}} )
			self:add_child(line)
			bbox_lines[#bbox_lines+1] = line
		end
		self.__bbox_lines = bbox_lines
	end

	for _, line in pairs(self.__bbox_lines) do
		line:set_visible(visible)
	end
end

return sprite