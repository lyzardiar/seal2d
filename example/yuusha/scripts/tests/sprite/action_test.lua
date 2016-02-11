local test = {}

local action = require "seal.action"

function test.sprite()
	local sprite = require "seal.sprite"
	local root = sprite.new()
	root:set_texture("res/white.png")
	root:set_size(100, 100)
	root:set_pos(0, 0)
	root:set_anchor(0, 0)

	local move = action.new_tween_loop(2, 1000, root, {x = 300, y=300, rotation = 360} )
	local call = action.call(function()
			print('move fin.')
		end)
	-- move:set_timescale(0.1)

	root:run_action(action.queue(move, call))
	-- root:add_tween(rotate)
	return root
end


return test