local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local nuk_core = require "nuklear_core"

local main_panel = {}

local menu_panel = require("editor.menu_panel")
local anim_panel
local prop_panel

function main_panel.draw()
	nuk_core.nuk_draw_start()

	menu_panel.draw(0)

	nuk_core.nuk_draw_end()
end

function main_panel.entry()
	sprite_frame.load_from_json("res/images/ui.json")
	sprite_frame.load_from_json("res/images/anim_pirate.json")
	sprite_frame.load_from_json("res/images/skeleton.json")

	menu_panel.load()
	-- root = sprite.new("ui.png", "smile_middle.png")
	root = sprite.new_container()
	root:set_pos(0, 0)

	return root.__cobj
end

return main_panel