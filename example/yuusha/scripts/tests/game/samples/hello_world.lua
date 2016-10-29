local hello_world = class("hello_world", function()
		return sprite.new_container()
	end)

function hello_world:ctor()
	local label = sprite.new_bmfont_label("hello seal2d :)", "res/fonts/animated.txt")
	label:set_pos(WINDOW_WIDTH/2, WINDOW_HEIGHT/2)
	self:add_child(label)

	print("construct hello world finished, self = ", self)
end

return hello_world