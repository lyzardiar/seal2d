local consts = require "seal.consts"
local game = {}

function game.load_shader(shader_index)

end

local root
function game.init()
	root = require ("tests.game.hello_world")
	-- root = require("tests.game.game_editor")

	return root.entry()
end

function game.draw()
	root.draw()
end

function game.pause()
	print('call game.pause()')
end

function game.resume()
	print('call game.resume()')
end

function game.reload()
	local loaded = {}
	for k,v in pairs(package.loaded) do
		loaded[#loaded+1] = k
	end

	for k,v in pairs(loaded) do
		package.loaded[v] = nil
	end

	local hello_world = require("tests.game.hello_world")
	hello_world.print_hello()
end

local EVENT_GAME_START = (0)
local EVENT_GAME_END   = (1)
function game.event(event_type, ...)
	if event_type == EVENT_GAME_START then
		print("Game Started")
	elseif event_type == EVENT_GAME_END then
		print("Game Ended")
	else
		assert(false, "unprocessed game event.")
	end
end

return game
