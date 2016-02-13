local seal_core = require "seal.core"
local engine = {
	-- root = nil,
}

-- local root

function engine.start(game)
	-- root = assert(game.start())
	-- engine.root = root
	game.start()

	seal_core.inject({
	 		update = assert(engine.update),
	 		draw   = assert(engine.draw),
	 		event  = assert(engine.event),

	 		pause  = assert(game.pause),
	 		resume = assert(game.resume),
	 	})
end

function engine.update(dt)
	-- print("update dt = ", dt)
end

function engine.draw()
	print("draw")
end

function engine.event(e)

end

return engine