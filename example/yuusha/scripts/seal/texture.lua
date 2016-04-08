local core = require "seal.texture"

local texture = {}

local cache = {}

local function gc(self)
	core.unload(self.__cobj)
end

local function new_texture(file_name)
	local obj = {}
	obj.__cobj = core.load(file_name)
	setmetatable(obj, {__gc = gc})
	return obj
end

function texture.load(file_name)
	local t = cache[file_name]
	if not t then
		t = new_texture(file_name)
		cache[file_name] = t
	end
	return t
end

return texture