local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"

local hello_world = {}

function hello_world.entry()
	sprite_frame.load_from_json("res/ui.json")

	-- TODO: redesign this interface(sprite.new)
	local root = sprite.new("cloud.png", "ui.png")

	return root.__cobj
end


return hello_world