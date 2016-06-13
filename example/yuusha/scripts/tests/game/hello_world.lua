local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local game = require "game"

local hello_world = {}
function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")

	-- TODO: redesign this interface(sprite.new)
	local root = sprite.new("smile_middle.png", "ui.png")
	game.root = root

	-- -- hello_world.test()

	local timer = require("seal.timer")

	local x, y = 0, 0
	root:set_pos(x, y)

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

			root:set_pos(x, y)
		end,

		loop = -1,
	}	
	return root.__cobj
end


function hello_world.print_hello()
end

return hello_world
