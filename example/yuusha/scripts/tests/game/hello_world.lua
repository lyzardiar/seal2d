local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local game = require "game"

local hello_world = {}

function hello_world.entry()
	local root = sprite.new_container()
	game.root = root

	local child = hello_world.create_sprite()
	root:add_child(child)
	return root.__cobj
end

function hello_world.create_sprite()
	sprite_frame.load_from_json("res/images/ui.json")

	local sprite = sprite.new("smile_middle.png", "ui.png")

	local timer = require("seal.timer")

	local x, y = 0, 0
	sprite:set_pos(x, y)

	local vx, vy = 10, 10
	timer.new {
		interval = 0,
		callback = function(dt)
		
			if x >= WINDOW_WIDTH - 200 or x <= -200 then
				vx = -vx
			end

			if y >= WINDOW_HEIGHT - 200 or y <= -200 then
				vy = -vy
			end

			x = x + vx * dt
			y = y + vy * dt

			sprite:set_pos(x, y)
		end,

		loop = -1,
	}	

	return sprite
end


function hello_world.print_hello()
end

return hello_world
