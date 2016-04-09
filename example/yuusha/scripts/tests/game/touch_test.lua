local test = {}

function test.sprite()
	local sprite = require "seal.sprite"
	local root = sprite.new()
	root:set_texture("res/white.png")
	root:set_size(400, 400)
	root:set_pos(WINDOW_WIDTH/2, WINDOW_HEIGHT/2)

	root:on_touch(function(event)
			if event.type == 'begin' then
				print('touch root begin', event.type, event.x, event.y, event.dx, event.dy)
			elseif event.type == 'move' then
				print('touch root move', event.type, event.x, event.y, event.dx, event.dy)
			else
				print('touch root end', event.type, event.x, event.y, event.dx, event.dy)
			end
			return true
		end)

	local child = sprite.new_image({
		
		})
	child:set_touch_swallow(true)
	child:on_touch(function(event)
			if event.type == 'begin' then
				print('touch child begin',event.type, event.x, event.y, event.dx, event.dy)
			elseif event.type == 'move' then
				print('touch child move',event.type, event.x, event.y, event.dx, event.dy)
			else
				assert(event.type == 'end')
				print('touch child end',event.type, event.x, event.y, event.dx, event.dy)
			end
			return true
		end)
	root:add_child(child)

	return root
end


return test