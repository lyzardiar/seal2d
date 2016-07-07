local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local game = require "game"

-- TODO: merge these 2 into 1 :)  
local nuk_node = require "seal.nuk_node"
local nuk_core = require "nuklear_core"

local NK_WINDOW_BORDER        = 1 << 0
local NK_WINDOW_BORDER_HEADER = 1 << 1
local NK_WINDOW_MOVABLE       = 1 << 2
local NK_WINDOW_SCALABLE      = 1 << 3
local NK_WINDOW_CLOSABLE      = 1 << 4
local NK_WINDOW_MINIMIZABLE   = 1 << 5
local NK_WINDOW_DYNAMIC       = 1 << 6
local NK_WINDOW_NO_SCROLLBAR  = 1 << 7
local NK_WINDOW_TITLE         = 1 << 8

local NK_BUTTON_DEFAULT = 0
local NK_BUTTON_REPEATER = 1

local hello_world = {}

function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")
	sprite_frame.load_from_json("res/images/anim_pirate.json")

	local root = sprite.new("ui.png", "smile_middle.png")
	root:register_handler(function()
		end)
	root:set_pos(0, 0)

	-- local clip = sprite.new_clip(0, 0, 150, 150)
	-- clip:set_pos(100, 100)
	-- root:add_child(clip)

	-- local child_1 = sprite.new("ui.png", "smile_middle.png")
	-- child_1:set_pos(-50, 0)
	-- clip:add_child(child_1)

	-- local child_2 = sprite.new("ui.png", "smile_middle.png")
	-- child_2:set_pos(100, 100)
	-- clip:add_child(child_2)

	-- we do some ugly performance test first. want to know how much cpu would it cost to call from C to Lua every frame.
	nuk_node.register(function()
			if(nuk_core.nk_begin("DemoTitle", {x = 50, y = 50, w = 200, h = 200}, 
					NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
				)) then

				nuk_core.nk_layout_row_static(30, 80, 1)
				if (nuk_core.nk_button_label("button", NK_BUTTON_DEFAULT)) then
					print("button pressed.")
				end

			end


			nuk_core.nk_end()
		end)

	return root.__cobj
end

function hello_world.print_hello()
end

return hello_world