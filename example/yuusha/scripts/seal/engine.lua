local engine_core = require "engine_core"
local timer = require "seal.timer"
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
	 		event  = assert(game.event),

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

	timer.update(dt)
end

function engine.draw()
	
end

return engine