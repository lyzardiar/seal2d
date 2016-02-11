local test = {}

local sprite = require "seal.sprite"
local action = require "seal.action"

local ball = class("ball", sprite)

function ball:ctor(vx, vy, ...)
	ball.__super.ctor(self, sprite.SPRITE_TYPE_IMAGE, ...)

	self.vx, self.vy = vx, vy
end

function ball:update(dt)
	local dx, dy = self.vx*dt/1000.0, self.vy*dt/1000.0
	local x, y = self.x + dx, self.y + dy

	self:set_pos(x, y)
	if x + dx > WINDOW_WIDTH or x + dx < 0 then
		self.vx = -self.vx
	end

	if y + dy > WINDOW_HEIGHT + dy or y + dy < 0 then
		self.vy = -self.vy
	end

end

function ball:get_speed()
	return self.vx, self.vy
end

function test.sprite()
	sprite.new_sheet("res/atlas_example.png", "atlas_example")

	local root = sprite.new_container{ width = WINDOW_WIDTH, height = WINDOW_HEIGHT }

	local total_ball = 500
	for i = 1, total_ball do
		local b = ball.new(math.random(0, 200), math.random(0, 200), {x = WINDOW_WIDTH/2, y = WINDOW_HEIGHT/2, frame_name = "smile.png"})
		root:add_child(b)
	end

	-- -- -- sprites move from center to four corners, stop a while, then back to center.
	-- -- for i, v in ipairs(sprites) do
	-- -- 	local spr = sprite.new_image {frame_name = v.start_x, v.start_y} 
	-- -- 	spr:set_pos(v.start_x, v.start_y)

	-- -- 	local move = action.tween(-1, 1000, spr, {x = v.end_x, y = v.end_y, rotation = 360} )
	-- -- 			print('move fin.')	-- 	local call = action.call(function()

	-- -- 		end)

	-- -- 	spr:run_action(action.queue(move, call))∂

	-- -- 	root:add_child(spr)
	-- -- end

	-- add a label in top center
	local name = sprite.new_ttf {
			text = "Complex Example",
			font_path = "res/fonts/Marker Felt.ttf",
			font_size = 24,
			x = WINDOW_WIDTH/2,
			y = WINDOW_HEIGHT,
		}
	name:set_anchor(0.5, 1)
	root:add_child(name)	

	return root
end


return test