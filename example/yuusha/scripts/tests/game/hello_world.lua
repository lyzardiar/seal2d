local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local game = require "game"

local hello_world = {
	i = "this is a copy",
}
function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")

	-- TODO: redesign this interface(sprite.new)
	local root = sprite.new("smile_middle.png", "ui.png")
	game.set_root(root)

	hello_world.print_hello()

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

	root:set_scale(2)
	root:set_rotation(90)
	return root.__cobj
end

function hello_world.print_hello()
	print("say some thing that is different. " .. hello_world.i)
end

-- local luaunit = require "luaunit"
-- function hello_world.test()
	-- require("tests.unit_test.simple_oop_test")()
	-- os.exit( luaunit.LuaUnit.run("-v") )
-- end


return hello_world