local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local timer = require "seal.timer"
local game = require "game"
local util = require "util"
local consts = require "consts"

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

local root
local function sprite_basic_test()
	print("run sprite_basic_test")
	root:cleanup()

	local sprites = {
		{ frame = "skeleton_1.png"},
		{ frame = "skeleton_1.png", scale = 0.5 },
		{ frame = "skeleton_1.png", scale = 1 },
		-- { frame = "attack_0.png", pos = {x = 400, y = 0}, color = {0, 255, 0, 255}},
	}

	local child = {}

	local x, y = 0, 0

	local last_width
	for i = 1, #sprites do
		local data = sprites[i]
		local s = sprite.new("skeleton.png", data.frame)
		s:set_pos(x, y)

		print("size = ", s:get_size())
		x = x + s:get_size()

		s:set_scale(data.scale or 1.0)
		if data.color then
			s:set_color(data.color[1], data.color[2], data.color[3], data.color[4])
		end

		root:add_child(s)
		child[#child+1] = s
	end

	-- child[1]:set_color(255, 0, 0, 255)
	-- child[2]:set_color(0, 255, 0, 255)
	-- child[3]:set_color(0, 0, 255, 255)
	-- child[4]:set_color(255, 255, 255, 128)
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

	local total = 5
	for i = 1, 5 do
		local s = sprite.new("anim_pirate.png", "attack_0.png")
		s:set_pos(i * 100, 0)
		s:register_handler(function(event_type, ...)
				print(string.format("sprite %d: ", i))
				if event_type == 'touch' then
					touch_handler(...)
				end
			end)
		root:add_child(s)
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
	{name = "sprite basic test", create_func = sprite_basic_test }, 
	{name = "sprite animation test", create_func = sprite_animation_test},
	{name = "sprite clip test", create_func = sprite_clip_test},
	{name = "sprite touch test", create_func = sprite_event_test},
	{name = "bmfont load test", create_func = bmfont_load_test},
	{name = "bunny test", create_func = bunny_test},
	{name = "multi texture test", create_func = multi_texture_test},
}

local function draw_gui()
		-- we do some ugly performance test first. want to know how much cpu would it cost to call from C to Lua every frame.
	nuk_node.register(function()
			if(nuk_core.nk_begin("Example", {x = WINDOW_WIDTH - 200, y = 50, w = 200, h = WINDOW_HEIGHT - 200/2}, 
					NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
				)) then

				-- the layout size should never be larger than the panel size.....
				nuk_core.nk_layout_row_static(30, 150, 1)

				for i = 1, #tests do
					local t = tests[i]
					if (nuk_core.nk_button_label(t.name, NK_BUTTON_DEFAULT)) then
						t.create_func()
					end
				end

			end

			nuk_core.nk_end()
		end)
end


function hello_world.entry()
	sprite_frame.load_from_json("res/images/ui.json")
	sprite_frame.load_from_json("res/images/anim_pirate.json")
	sprite_frame.load_from_json("res/images/skeleton.json")

	root = sprite.new_container()
	root:set_pos(0, 0)

	draw_gui()

	return root.__cobj
end

function hello_world.print_hello()
end

return hello_world