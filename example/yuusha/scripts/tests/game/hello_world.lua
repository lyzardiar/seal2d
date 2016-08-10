local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local timer = require "seal.timer"
local game = require "game"
local util = require "util"
local consts = require "consts"
local nuklear = require "nuklear"

local hello_world = {}

local root

local function load_sprite(texture, data, x, y)
	local s = sprite.new(texture, data.frame)
	s:set_pos(x, y)

	s:set_scale(data.scale or 1.0)
	if data.color then
		s:set_color(data.color[1], data.color[2], data.color[3], data.color[4])
	end

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
		s = load_sprite("skeleton.png", sprites[i], x, y)
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
		{ frame = "skeleton_2.png", scale = 2},
		{ frame = "skeleton_3.png", rotation = 90 },
		{ frame = "skeleton_4.png", rotation = 360, scale = 2 },
	}

	load_skeltons(sprites, 0, WINDOW_HEIGHT/2)
end

local function sprite_anchor_test()
	print("run sprite_anchor_test")
	root:cleanup()
	local sprites = {
		{ frame = "skeleton_1.png" },

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

local function sprite_event_test()
	print("run sprite_clip_test")
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
	root:cleanup()

	local s1 = sprite.new("ui.png", "smile_middle.png")
	root:add_child(s1)

	local s2 = sprite.new("anim_pirate.png", "attack_0.png")
	s2:set_pos(200, 200)
	root:add_child(s2)
end

local tests = {
	{name = "sprite srt test", create_func = sprite_srt_test },
	{name = "sprite anchor test", create_func = sprite_anchor_test },
	{name = "sprite animation test", create_func = sprite_animation_test},
	{name = "sprite clip test", create_func = sprite_clip_test},
	{name = "sprite touch test", create_func = sprite_event_test},
	{name = "bmfont load test", create_func = bmfont_load_test},
	{name = "bunny test", create_func = bunny_test},
	{name = "multi texture test", create_func = multi_texture_test},
}

local function draw_gui()

	print("call draw_gui")
	nuklear.nuk_draw_start()

	if(nuklear.nk_begin("Example", 
		{x = WINDOW_WIDTH - 200, y = 50, w = 200, h = WINDOW_HEIGHT - 200/2},
			nuklear.NK_WINDOW_BORDER      |
			nuklear.NK_WINDOW_MOVABLE     |
			nuklear.NK_WINDOW_SCALABLE    |
			nuklear.NK_WINDOW_MINIMIZABLE |
			nuklear.NK_WINDOW_TITLE
		)) then

		-- the layout size should never be larger than the panel size.....
		nuklear.nk_layout_row_static(30, 150, 1)

		for i = 1, #tests do
			local t = tests[i]
			if (nuklear.nk_button_label(t.name, nuklear.NK_BUTTON_DEFAULT)) then
				t.create_func()
			end
		end

	end
	nuklear.nk_end()

	nuklear.nuk_draw_end()
end

function hello_world.draw()
	local platform = require ("platform_core").get_platform()
	if platform == 'mac' or platform == 'win' then
		draw_gui()
	end
end

function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")
	sprite_frame.load_from_json("res/images/anim_pirate.json")
	sprite_frame.load_from_json("res/images/skeleton.json")

	-- root = sprite.new("ui.png", "smile_middle.png")
	root = sprite.new_container()
	root:set_pos(0, 0)

	local platform = require ("platform_core").get_platform()
	if platform == 'mac' or platform == 'win' then
		nuklear.init()
	end

	return root.__cobj
end

return hello_world