local seal_core = require "seal.core"
local sdl_render = require "sdl.render"

local engine = {
	window    = seal_core.get_window(),
	gl_render = seal_core.get_render(),
	platform  = seal_core.get_platform(),
	root = nil,
}

local root

function engine.start(game)
	root = assert(game.start())
	engine.root = root

	seal_core.inject({
	 		update = assert(engine.update),
	 		draw   = assert(engine.draw),
	 		event  = assert(engine.event),

	 		pause  = assert(game.pause),
	 		resume = assert(game.resume),
	 	})
end

function engine.update(dt)
	engine.root:update(dt)
end

function engine.draw()
	sdl_render.clear()	
	root:draw()
	sdl_render.present()
end

local event = require "seal.event"
function engine.event(e)
	event.mouse(e, root)
end

return engine