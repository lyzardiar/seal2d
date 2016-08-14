local sprite = require "seal.sprite"
local drum_meastro = {}

local root
local score

function drum_meastro.draw()
    score:draw()
end

function drum_meastro.entry()
    score = require("tests.drum_play.score").new("res/scores/its_my_life.json")

    root = sprite.new_container()
    root:set_pos(0, 0)
    return root.__cobj
end

return drum_meastro