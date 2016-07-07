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

local root
local function sprite_basic_test()
	print("run sprite_basic_test")
	root:remove_all_child()


	local total = 5
	for i = 1, 5 do
		local s = sprite.new("anim_pirate.png", "attack_0.png")
		s:set_pos(i * 100, 0)
		root:add_child(s)
	end
end

local function sprite_animation_test()
	print("run sprite_animation_test")

	root:remove_all_child()

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

	root:remove_all_child()

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

	root:remove_all_child()

	local function event_handler(event)
		print("event = ", event)


	end

	local total = 5
	for i = 1, 5 do
		local s = sprite.new("anim_pirate.png", "attack_0.png")
		s:set_pos(i * 100, 0)
		s:register_handler(function()
				print(string.format("sprite %d: ", i))

			end)
		root:add_child(s)
	end 
end


local tests = {
	{name = "sprite basic test", create_func = sprite_basic_test }, 
	{name = "sprite animation test", create_func = sprite_animation_test},
	{name = "sprite clip test", create_func = sprite_clip_test},
	{name = "sprite touch test", create_func = sprite_event_test},
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

	root = sprite.new_container()
	root:register_handler(function()
		end)
	root:set_pos(0, 0)

	draw_gui()

	return root.__cobj
end

function hello_world.print_hello()
end

return hello_world