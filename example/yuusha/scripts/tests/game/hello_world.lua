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
	for i = 0, 4 do
		local child = sprite.new("anim_pirate.png", string.format("attack_%d.png", i))
		child:set_pos(i* 100, 200)
		root:add_child(child)

		sprites[#sprites+1] = child
	end

	-- local frames = {}
	-- for i = 0, 4 do
	-- 	local name = "attack_" .. i .. ".png"
	-- 	local f = sprite_frame.get( "anim_pirate.png", name)
	-- 	frames[#frames+1] = f
	-- end

	-- root:set_pos(200, 200)
	-- sprites[1]:set_anim(frames)

	-- local timer = require("seal.timer")
	-- local vx, vy = 10, 10
	-- timer.new {
	-- 	interval = 0,
	-- 	callback = function(dt)
	-- 		local x, y = root:get_pos()
	-- 		x = x + vx * dt
	-- 		root:set_pos(x, y)
	-- 	end,

	-- 	loop = -1,
	-- }



	return root.__cobj
end

function hello_world.print_hello()
end

return hello_world
