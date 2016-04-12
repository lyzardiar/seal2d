--[[
	spriteframe never frees, it will not take up too much memory.
]]

local platform_core = require "platform_core"
local sprite_core = require "sprite_core"
local texture_core = require "texure_core"

local cjson = require "cjson"


local sprite_frame = {}

local function gc(self)
	sprite_core.unload_spriteframe(self.__cobj)
end

local frame_cache = {}
local frame_id_name_map = {}

local function new_frame(...)
	local obj = {}
	obj.__cobj = sprite_core.spriteframe_load(...)
	print("new_frame obj.__cobj = ", obj.__cobj)
	setmetatable(obj, {__gc = gc})
	return obj
end

function sprite_frame.load_from_json(json_path)
	local data = platform_core.read_s(json_path)
	local frames = cjson.decode(data)
	local frame_data = frames.frames
	local meta = frames.meta
	
	-- TODO : hard code here, fix with the path searching.

	local tex_id = texture_core.load_from_cache("res/images/" .. meta.image)

	-- local texture_filename = meta.
	for frame_name, data in pairs(frame_data) do
		local frame = new_frame(data, meta)
		sprite_core.spriteframe_set_texture_id(frame.__cobj, tex_id)
		frame_cache[meta.image .. "-" .. frame_name] = frame
	end


end

function sprite_frame.get(frame_name, atlas_name)
	local f = frame_cache[atlas_name .. "-" .. frame_name]
	if not f then
		print_r(frame_cache)
		assert(false, "frame: %s cannot be found.", atlas_name .. "-" .. frame_name)
	end
	return f
end

return sprite_frame