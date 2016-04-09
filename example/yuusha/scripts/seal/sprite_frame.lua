--[[
	spriteframe never frees, it will not take up too much memory.
]]

local platform = require "seal.platform"
local core = require "seal.sprite_core"
local cjson = require "cjson"

local texture_core = require "seal.texture_core"

local sprite_frame = {}

local function gc(self)
	core.unload_spriteframe(self.__cobj)
end

local frame_cache = {}
local frame_id_name_map = {}

local function new_frame(...)
	local obj = {}
	obj.__cobj = core.spriteframe_load(...)
	return setmetatable(obj, {__gc = gc})
end

function sprite_frame.load_from_json(json_path)
	local data = platform.read_s(json_path)
	local frames = cjson.decode(data)
	local frame_data = frames.frames
	local meta = frames.meta
	print_r(meta)
	
	-- TODO : hard code here, fix with the path searching.

	local tex_id = texture_core.load_from_cache("res/" .. meta.image)

	-- local texture_filename = meta.
	for frame_name, data in pairs(frame_data) do
		print("frame_name = ", frame_name)
		print("type of data = ", type(data))
		print_r(data)
		local frame = new_frame(data)
		core.spriteframe_set_texture_id(frame.__cobj, tex_id)
		frame_cache[meta.image .. "-" .. frame_name] = frame
	end
end

function sprite_frame.get(frame_name, atlas_name)
	return frame_cache[atlas_name .. "-" .. meta.image]
end

return sprite_frame