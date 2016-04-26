local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local game = require "game"

local hello_world = {}
function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")

	-- TODO: redesign this interface(sprite.new)
	local root = sprite.new("smile_middle.png", "ui.png")
	game.root = root

	hello_world.test()

	local pos = {
		{x = 100, y = 0},
		{x = 100, y = 100},
		{x = 0, y = 100}
	}

	-- local children = {}
	-- for i = 1, 3 do
		-- local child = sprite.new("test_trimed.png", "ui.png")
		-- child:set_pos(pos[i].x, pos[i].y)
		-- root:add_child(child)
		-- children[i] = child
	-- end
	-- children[1]:set_scale(2)

	-- root:set_scale(2)
	root:set_rotation(30)
	return root.__cobj
end

local luaunit = require "luaunit"
function hello_world.test()
	require("tests.unit_test.simple_oop_test")()
	-- os.exit( luaunit.LuaUnit.run("-v") )
end


return hello_world