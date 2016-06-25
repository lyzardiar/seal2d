--[[
	spriteframe never frees, it will not take up too much memory.
]]

local platform_core = require "platform_core"
local sprite_core = require "sprite_core"
local texture_core = require "texure_core"

local cjson = require "cjson"

local sprite_frame = {}

local function make_frame_name(...)
	return string.format("%s-%s", ...)
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
		local key = make_frame_name(meta.image, frame_name)
		local frame = sprite_core.load_sprite_frame(data, meta, key)
		sprite_core.set_frame_texture_id(frame, tex_id)
	end
end

function sprite_frame.get(...)
	return sprite_core.get_frame_from_cache(make_frame_name(...))
end

return sprite_frame