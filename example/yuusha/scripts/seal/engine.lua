local engine_core = require "engine_core"
local consts = require "seal.consts"
local engine = {
	-- root = nil,
}

-- local root

function engine.start(game)
	-- root = assert(game.start())
	-- engine.root = root
	-- game.init()

	engine_core.inject({
	 		update = assert(engine.update),
	 		draw   = assert(engine.draw),
	 		event  = assert(engine.event),

	 		init   = assert(game.init),
	 		pause  = assert(game.pause),
	 		resume = assert(game.resume),
	 	})
end

local total = 0
local cnt = 0
function engine.update(dt)
	if dt < 0 then 
		return
	end

	total = total + dt
	if total > 1 then
		total = 0
		cnt = cnt + 1
		-- print("update cnt = ", cnt)
	end
end

function engine.draw()

end

local TOUCH_BEGIN  = consts.TOUCH_BEGIN
local TOUCH_MOVE   = consts.TOUCH_MOVE
local TOUCH_END    = consts.TOUCH_END
local TOUCH_CANCEL = consts.TOUCH_CANCEL

function engine.event(e)
	if e.type == TOUCH_BEGIN then
		print(string.format("begin, (%.2f, %.2f)", e.x, e.y))
	elseif e.type == TOUCH_MOVE then
		print(string.format("move, (%.2f, %.2f)", e.x, e.y))
	elseif e.type == TOUCH_END then
		print(string.format("end, (%.2f, %.2f)", e.x, e.y))
	elseif e.type == TOUCH_CANCEL then
		print(string.format("cancel, (%.2f, %.2f)", e.x, e.y))
	else
		assert(false, "invalid event type = ", e.type)
	end
end

return engine