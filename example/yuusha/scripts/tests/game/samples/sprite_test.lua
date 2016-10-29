local nuk_node = require "seal.nuk_node"
local common = require "tests.game.samples.sample_common"

local sprite_test = class("sprite_test", function()
    return sprite.new_container()
end)

local function load_srt(self)
    local sprites = {
        { frame = "skeleton_1.png" },
        { frame = "skeleton_2.png", scale = 1.5},
        { frame = "skeleton_3.png", rotation = 90 },
        { frame = "skeleton_4.png", rotation = 360, scale = 2 },
    }

    common.load_skeltons(self, "skeleton.png", sprites, 0, WINDOW_HEIGHT/2)
end

local function load_anchor(self)
    local sprites = {
        {
            frame = "skeleton_1.png",
            anchor = { x = 0, y = 0},
            pos = { x = 0, y = 0},
        },
        {
            frame = "skeleton_1.png",
            anchor = {x = 0.5, y = 0.5},
            pos = {x = WINDOW_WIDTH/2, y = WINDOW_HEIGHT/2}
        },
        {
            frame = "skeleton_1.png",
            anchor = {x = 1, y = 0},
            pos = {x = WINDOW_WIDTH, y = 0}
        },
        {   frame = "skeleton_1.png",
            anchor = {x = 1, y = 1},
            pos = {x = WINDOW_WIDTH, y = WINDOW_HEIGHT}
        },
        {
            frame = "skeleton_1.png",
            anchor = {x = 0, y = 1},
            pos = {x = 0, y = WINDOW_HEIGHT}
        },
    }

    common.load_skeltons(self, "skeleton.png", sprites)
end

local function load_anim(self)
    local frames = {}
    for i = 0, 4 do
        local name = string.format("attack_%d.png", i)
        local f = assert(sprite_frame.get("anim_pirate.png", name))
        frames[#frames+1] = f
    end

    local s = sprite.new("anim_pirate.png", "attack_0.png")

    s:set_pos(WINDOW_WIDTH/2, WINDOW_HEIGHT/2)
    s:set_anim(frames)
    s:set_anim_interval(1)

    self:add_child(s)
end

function sprite_test:ctor()
    self.bar = nuk_node.new()
end

function sprite_test:on_enter()
	print("sprite_test on_enter")
end

function sprite_test:on_exit()
	print("sprite_test on_exit")
end

local test_cases = {
    {name = "srt test", load_func = load_srt},
    {name = "anchor test", load_func = load_anchor},
    {name = "animation test", load_func = load_anim},
    {name = "spine test", load_func = load_spine },
    {name = "glyph test", load_func = load_glyph },
    {name = "zorder test", load_func = load_zorder},
}

local BH = 40 --button height
local RH = 100 -- row height
function sprite_test:draw()
    local bar = self.bar

    if (bar:nk_begin("bar",
                     {
                        x = 0,
                        y = WINDOW_HEIGHT - (RH - BH),
                        w = WINDOW_WIDTH,
                        h = RH
                     },
                     nuk_node.NK_WINDOW_MOVABLE )) then

        -- the layout size should never be larger than the panel size.....
        bar.nk_layout_row_dynamic(BH, #test_cases)

        for i = 1, #test_cases do
            local t = test_cases[i]
            if (bar.nk_button_label(t.name, bar.NK_BUTTON_DEFAULT)) then
                self:cleanup()
                t.load_func(self)
            end
        end
    end
    bar.nk_end()
end

return sprite_test