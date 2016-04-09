local test = {}

function test.sprite()
	local sprite = require "seal.sprite"
	local root = sprite.new_ttf {
			text = "sample",
			font_path = "res/fonts/Marker Felt.ttf",
			font_size = 24,
			x = 100,
			y = 100
		}
	
	root:set_text("another")
	collectgarbage()
	return root
end


return test 