local sprite_frame = require "seal.sprite_frame"

local hello_world = {}

function hello_world.sprite()
	sprite_frame.load_from_json("res/ui.json")
end


return hello_world