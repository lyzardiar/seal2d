local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local game = require "game"

local hello_world = {}

function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")
	sprite_frame.load_from_json("res/images/anim_pirate.json")

	local root = sprite.new_container()
	game.root = root

	local sprites = {}
	for i = 1, 2 do
		local child = sprite.new("smile_middle.png", "ui.png")
		child:set_pos((i-1)*50, 0)
		root:add_child(child)

		sprites[#sprites+1] = child
	end

	local frames = {}
	for i = 0, 4 do
		local name = "attack_" .. i .. ".png"
		local f = sprite_frame.get(name, "anim_pirate.png")
		frames[#frames+1] = f
	end

	sprites[1]:set_anim(frames)

	local timer = require("seal.timer")
	local vx, vy = 10, 10
	timer.new {
		interval = 0,
		callback = function(dt)
			local x, y = root:get_pos()
			x = x + vx * dt
			root:set_pos(x, y)
		end,

		loop = -1,
	}



	return root.__cobj
end

function hello_world.print_hello()
end

return hello_world
