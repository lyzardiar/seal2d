local common = require "tests.game.samples.sample_common"

local sprite_srt = class("sprite_srt", function()
    return sprite.new_container()
end)

function sprite_srt:ctor()
	local sprites = {
        { frame = "skeleton_1.png" },
        { frame = "skeleton_2.png", scale = 1.5},
        { frame = "skeleton_3.png", rotation = 90 },
        { frame = "skeleton_4.png", rotation = 360, scale = 2 },
    }

    common.load_skeltons(self, "skeleton.png", sprites, 0, WINDOW_HEIGHT/2)
end

function sprite_srt:on_enter()
	print("sprite_srt on_enter")
end

function sprite_srt:on_exit()
	print("sprite_srt on_exit")
end


return sprite_srt
