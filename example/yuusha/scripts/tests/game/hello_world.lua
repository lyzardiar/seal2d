local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local game = require "game"

local hello_world = {}

function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")
	sprite_frame.load_from_json("res/images/anim_pirate.json")


	local root = sprite.new("ui.png", "smile_middle.png")
	root:set_pos(0, 0)

	local clip = sprite.new_clip(0, 0, 150, 150)
	clip:set_pos(100, 100)
	root:add_child(clip)

	local child_1 = sprite.new("ui.png", "smile_middle.png")
	child_1:set_pos(-50, 0)
	clip:add_child(child_1)

	local child_2 = sprite.new("ui.png", "smile_middle.png")
	child_2:set_pos(100, 100)
	clip:add_child(child_2)

	return root.__cobj
end

function hello_world.print_hello()
end

return hello_world
-- 