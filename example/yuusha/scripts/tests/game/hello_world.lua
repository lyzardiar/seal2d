local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local timer = require "seal.timer"
local game = require "game"
local util = require "util"
local consts = require "consts"
local nuklear = require "nuklear_core"


local hello_world = {}

local root
local menu

local function load_sprite(texture, data, x, y)
	local s = sprite.new(texture, data.frame)
	s:set_pos(x, y)

	s:set_scale(data.scale or 1.0)
	-- if data.color then
		-- s:set_color(data.color[1], data.color[2], data.color[3], data.color[4])
	-- end

	if data.rotation then
		s:set_rotation(data.rotation)
	end

	if data.anchor then
		s:set_anchor(data.anchor.x, data.anchor.y)
	end

	if data.pos then
		s:set_pos(data.pos.x, data.pos.y)
	end

	if data.test_description then
		s.test_description = data.test_description
	end
	return s
end

local function load_skeltons(sprites, x0, y0)
	local child = {}
	local x, y = x0 or 0, y0 or 0
	local last_width
	for i = 1, #sprites do
		s = load_sprite("skeleton.png", sprites[i], x + 100, y)
		x = x + s:get_size()
		root:add_child(s)
		child[#child+1] = s
	end
	return child
end

local function sprite_srt_test()
	print("run sprite_srt_test")
	root:cleanup()

	local sprites = {
		{ frame = "skeleton_1.png" },
		{ frame = "skeleton_2.png", scale = 1.5},
		{ frame = "skeleton_3.png", rotation = 90 },
		{ frame = "skeleton_4.png", rotation = 360, scale = 2 },
	}

	load_skeltons(sprites, 0, WINDOW_HEIGHT/2)
end

local function sprite_anchor_test()
	print("run sprite_anchor_test")
	root:cleanup()
	local sprites = {
		{ frame = "skeleton_1.png",
			anchor = { x = 0, y = 0},
			pos = { x = 0, y = 0},
		 },

		{ 	frame = "skeleton_1.png",
			anchor = {x = 0.5, y = 0.5},
			pos = {x = WINDOW_WIDTH/2, y = WINDOW_HEIGHT/2} },

		{ 	frame = "skeleton_1.png",
			anchor = {x = 1, y = 0},
			pos = {x = WINDOW_WIDTH, y = 0} },


		{ 	frame = "skeleton_1.png",
			anchor = {x = 1, y = 1},
			pos = {x = WINDOW_WIDTH, y = WINDOW_HEIGHT} },

		{ 	frame = "skeleton_1.png",
			anchor = {x = 0, y = 1},
			pos = {x = 0, y = WINDOW_HEIGHT} },
	}

	load_skeltons(sprites)
end

local function sprite_animation_test()
	print("run sprite_animation_test")

	root:cleanup()

	local frames = {}
	for i = 0, 4 do
		local name = string.format("attack_%d.png", i)
		local f = assert(sprite_frame.get("anim_pirate.png", name))
		frames[#frames+1] = f
	end

	local s = sprite.new("anim_pirate.png", "attack_0.png")

	s:set_pos(100,0)
	s:set_anim(frames)
	s:set_anim_interval(1)

	root:add_child(s)
end

local function sprite_clip_test()
	print("run sprite_clip_test")

	root:cleanup()

	local clip = sprite.new_clip(0, 0, 200, 200)

	local sprites = {
		{x = 0,   y = 0, 	frame = "attack_0.png"},
		{x = 150, y = 150, 	frame = "attack_1.png"},
		{x = 250, y = 150, 	frame = "attack_2.png"},
	}

	for i = 1, #sprites do
		local data = sprites[i]
		local s = sprite.new("anim_pirate.png", "attack_0.png")
		s:set_pos(data.x, data.y)
		clip:add_child(s)
	end

	clip:set_pos(100, 0)

	root:add_child(clip)
end

local function sprite_primitive_test()
	print("run sprite_primitive_test")
	root:cleanup()

	local container = sprite.new_container()
	container:set_pos(100, 100)

	local g = {0, 0, 160, 0, 160, 116, 0, 116}
	local lines = {
		{g[1], g[2], g[3], g[4]},
		{g[3], g[4], g[5], g[6]},
		{g[5], g[6], g[7], g[8]},
		{g[7], g[8], g[1], g[2]},
	}
	for i = 1, #lines do
		local line = sprite.new_primitive("L", {
										vertex = lines[i],
										width = 2.0,
										color = {30, 0, 0, 255}} )
		container:add_child(line)
	end

	local rect = sprite.new_primitive("R", {
										rect = {0, 0, 100, 200},
										fill_color = {255, 0, 0, 255}
										})
	rect:set_pos(200, 200)
	container:add_child(rect)

	root:add_child(container)
end

local function sprite_spine_test()
	print("run sprite_spine_test")
	root:cleanup()

	local test_cases = {
		{
			atlas = "res/spine_anim/spineboy.atlas",
		 	json = "res/spine_anim/spineboy.json",
		 	anim_name = 'walk', x = 30, y = 0, scale = 0.2
	    },
		{
			atlas = "res/spine_anim/spineboy.atlas",
		 	json = "res/spine_anim/spineboy.json",
			anim_name = 'run', x = WINDOW_WIDTH/2 - 30, y = WINDOW_HEIGHT/2, scale = 0.2
		},
		{
			atlas = "res/spine_anim/powerup.atlas",
		 	json = "res/spine_anim/powerup.json",
			anim_name = 'animation', x = WINDOW_WIDTH/2 - 30, y = WINDOW_HEIGHT/2 + 100, scale = 0.2
		},
	}
	for i = 1, #test_cases do
		local c = test_cases[i]
		local spine = sprite.new_spine(c.atlas, c.json, c.scale)
		spine:set_pos(c.x, c.y)
		spine:set_spine_anim(c.anim_name, 0, true)
		root:add_child(spine)
	end
end

local function sprite_loader_test()
	print("run sprite_loader_test")
	root:cleanup()

	local loader = require("editor_runtime.seal_loader")
	local s = loader.load("res/game/main.json")
	root:add_child(s)
end

local function sprite_glyph_test()
	print("run sprite_loader_test")
	root:cleanup()

	local s = sprite.new("anim_pirate.png", "attack_0.png")
	s:set_pos(100, 100)
	s:set_bbox_visible(true)
	root:add_child(s)

	local s2 = sprite.new("anim_pirate.png", "attack_0.png")
	s2:set_pos(300, 100)
	s2:set_bbox_visible(false)
	root:add_child(s2)
end

local function sprite_action_test()
	print("run sprite_action_test")
	root:cleanup()
	local s = sprite.new("anim_pirate.png", "attack_0.png")
	s:set_pos(0, 0)
	root:add_child(s)

	local action = require("action_core")
	local move = action.move_to(1, 100, 100)
	local back = action.move_to(1, 0, 0)
	local finish = action.call_func(function()
			print("finished.")
		end)
	local seq = action.sequence({move, back, finish})
	s:run_action(seq)
	-- local ease_move = action.ease_in(move, 5)
	-- s:run_action(ease_move)
end

local function sprite_event_test()
	print("run sprite_event_test")
	root:cleanup()

	local touch_event_names = {
		[0] = "begin",
		[1] = "moved",
		[2] = "ended",
		[3] = "canceled",
	}
	local function touch_handler(event, x, y)
		print("event = ", touch_event_names[event], x, y)
	end

	local spritesData = {
		{frame = "skeleton_1.png", pos = {x = 100, y = 500}, test_description = "normal"},
		{frame = "skeleton_1.png", pos = {x = 300, y = 500}, anchor = {x = 0, y = 0}, test_description = "anchor point = (0, 0)"},
		{frame = "skeleton_1.png", pos = {x = 300, y = 500}, anchor = {x = 0, y = 1}, test_description = "anchor point = (0, 1)"},
		{frame = "skeleton_1.png", pos = {x = 300, y = 500}, anchor = {x = 1, y = 0}, test_description = "anchor point = (1, 0)"},
		{frame = "skeleton_1.png", pos = {x = 300, y = 500}, anchor = {x = 1, y = 1}, test_description = "anchor point = (1, 1)"},
		{frame = "skeleton_1.png", pos = {x = 100, y = 300}, anchor = {x = 0, y = 0}, scale = 2, test_description = "set scale 2"},
		{
			frame = "skeleton_1.png", pos = {x = 200, y = 300}, anchor = {x = 0, y = 0},
			rotation = 90, test_description = "set rotation 90"
		},
		{
			frame = "skeleton_1.png", pos = {x = 400, y = 300}, anchor = {x = 0, y = 0},
			scale = 2, rotation = 90, test_description = "scale=2 and rotation=90"
		},
		{
			frame = "skeleton_1.png", pos = {x = 100, y = 200}, anchor = {x = 1, y = 1},
			scale = 0.5, rotation = 120, test_description = "scale=0.5 and rotation = 120"
		},
	}
	local sprite = load_skeltons(spritesData)

	for i=1,#sprite do
		sprite[i]:register_handler(function(event_type, ...)
				print(string.format("sprite: %d , test name: %s", i, sprite[i].test_description))
				if event_type == 'touch' then
					touch_handler(...)
				end
			end)
	end
end

local function bmfont_load_test()
	print("run bmfont_load_test")
	root:cleanup()

	local labels = {
		{ pos = {x = 0, y = 500}, label = "(the quick brown fox jumps over the lazy dog." },
		{ pos = {x = 0 , y = 0}, label = "h,ello world"},
		-- { pos = {x = 0 , y = 100}, label = "hello world", scale = 0.5},
	}

	for i = 1, #labels do
		local l = labels[i]
		local s = sprite.new_bmfont_label(l.label, "res/fonts/animated.txt")
		s:set_pos(l.pos.x, l.pos.y)
		if l.scale then
			s:set_scale(l.scale)
		end
		root:add_child(s)
	end

end

local function bunny_test()
	print("run bunny_test")
	root:cleanup()

	local label = sprite.new_bmfont_label("count: 0", "res/fonts/animated.txt")
	label:set_pos(0, 60)
    root:add_child(label)

    local count = 0
	local bunnies = {}
	local n = 500
	local function add_bunny(x, y)
		for i = 1, n do
			local b = sprite.new("ui.png", "smile_middle.png")
			b:set_pos(x, y)
			root:add_child(b)
		end
		count = count + n

		label:set_text(string.format("count: %d", count))
    end


    local isClick = false
	root:register_handler(function(event, t, x, y)
		if t == consts.TOUCH_END and isClick then
			add_bunny(x, y)
			print("add_bunny:", x, y)
			isClick = false
		elseif t == consts.TOUCH_BEGIN then
			isClick = true
		end
	end)

	local fps_label = sprite.new_bmfont_label("FPS: 0", "res/fonts/animated.txt")
    root:add_child(fps_label)
end

local function multi_texture_test()
	print("run multi_texture_test")
	root:cleanup()

	local s1 = sprite.new("ui.png", "smile_middle.png")

	local s2 = sprite.new("anim_pirate.png", "attack_0.png")
	s2:set_pos(200, 200)
	local s3 = sprite.new("anim_pirate.png", "attack_0.png")
	s3:set_pos(400, 200)

	local s4 = sprite.new("ui.png", "smile_middle.png")
	s4:set_pos(400, 0)
	root:add_child(s1)
	root:add_child(s2)
	root:add_child(s3)
	root:add_child(s4)
end

local naovg_draw

local function nanovg_test()
	print("run nanovg_test")
	root:cleanup()

	local nvg = require "nanovg_core"
	nanovg_draw = function()
		nvg.begin_frame()
		nvg.stroke_width(1.0)
		nvg.stroke_color(255, 0, 0, 255)
		nvg.begin_path()
		nvg.move_to(100, 100)
		nvg.line_to(100, 200)
		nvg.stroke()
		nvg.end_frame()
	end
end

local tests = {
	{name = "sprite srt test", create_func = sprite_srt_test },
	{name = "sprite anchor test", create_func = sprite_anchor_test },
	{name = "sprite animation test", create_func = sprite_animation_test},
	-- {name = "sprite clip test", create_func = sprite_clip_test},
	{name = "sprite primitive test", create_func = sprite_primitive_test },
	{name = "sprite touch test", create_func = sprite_event_test},
	{name = "sprite spine test", create_func = sprite_spine_test},
	{name = "sprite loader test", create_func = sprite_loader_test},
	{name = "sprite glyph test", create_func = sprite_glyph_test},
	{name = "sprite action test", create_func = sprite_action_test},
	{name = "bmfont load test", create_func = bmfont_load_test},
	{name = "bunny test", create_func = bunny_test},
	{name = "multi texture test", create_func = multi_texture_test},
	{name = "nanovg_test", create_func = nanovg_test },
}

local function draw_gui()
	local nuk_node = require "nuk_node"
	nuklear.nuk_draw_start()

	if(menu:nk_begin("tests",
		{x = WINDOW_WIDTH - 200, y = 50, w = 200, h = WINDOW_HEIGHT - 200/2},
			nuk_node.NK_WINDOW_BORDER      |
			nuk_node.NK_WINDOW_MOVABLE     |
			nuk_node.NK_WINDOW_SCALABLE    |
			nuk_node.NK_WINDOW_MINIMIZABLE |
			nuk_node.NK_WINDOW_TITLE
		)) then

		-- the layout size should never be larger than the panel size.....
		menu.nk_layout_row_static(30, 150, 1)

		for i = 1, #tests do
			local t = tests[i]
			if (menu.nk_button_label(t.name, menu.NK_BUTTON_DEFAULT)) then
				if t.name ~= 'nanovg_test' then
					nanovg_draw = nil
				end
				t.create_func()
			end
		end
	end
	menu.nk_end()

	nuklear.nuk_draw_end()
end

function hello_world.draw()
	local platform = require ("platform_core").get_platform()
	if device.is_pc() then
		draw_gui()
	end

	if nanovg_draw then
		nanovg_draw()
	end
end

function hello_world.entry()
	-- the sprite frame info is often very small.
	-- we don't need to release them anyway.
	sprite_frame.load_from_json("res/images/ui.json")
	sprite_frame.load_from_json("res/images/anim_pirate.json")
	sprite_frame.load_from_json("res/images/skeleton.json")

	root = sprite.new_container()
	-- root = sprite.new("anim_pirate.png", "attack_0.png")

	root:set_pos(0, 0)


	if device.is_pc() then
		local nuk_node = require "nuk_node"
		menu = nuk_node.new()
	else
		sprite_spine_test()
	end

	return root.__cobj
end

return hello_world