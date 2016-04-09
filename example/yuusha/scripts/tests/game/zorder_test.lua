local test = {}

function test.sprite()
	local sprite = require "seal.sprite"
	local root = sprite.new()
	root:set_texture("res/saber.png")
	root:set_pos(100, 100)
	root:set_rotation(20)

	local zorders = {100, 0, -1, 1}
	local x, y = 200, 200
	for i = 1, #zorders do
		local child = sprite.new()
		print('file name = ', string.format("res/saber_zorder_%d.png", zorders[i]))
		child:set_texture(string.format("res/saber_zorder_%d.png", zorders[i]))
		child:set_pos(x + 30*i, y + 30*i)
		child:set_zorder(zorders[i])
		root:add_child(child)
	end

	local x, y = 0, 0
	for i = 1, 1000 do
		x = (x + 10)
		if x > 1136 then
			x = 0
			y = y + 100
		end

		local batch = sprite.new()
		batch:set_pos(x, y)

		batch:set_texture(string.format("res/saber.png"))
		batch:set_zorder(2)
		root:add_child(batch)
	end

	local children = root:get_children()
	scene = root

end


return test