local engine_core = require "engine_core"
local timer = require "seal.timer"
local engine = {
}


function engine.start(game)
    print("start engine")
    engine_core.inject({
            update = assert(engine.update),
            
            draw   = assert(game.draw),
            event  = assert(game.event),
            init   = assert(game.init),
            pause  = assert(game.pause),
            resume = assert(game.resume),
        })
    print("start completed")
end

local total = 0
local cnt = 0
function engine.update(dt)
    if dt < 0 then 
        return
    end

    timer.update(dt)
end

return engine