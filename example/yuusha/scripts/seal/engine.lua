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

function engine.event(e)

end

return engine