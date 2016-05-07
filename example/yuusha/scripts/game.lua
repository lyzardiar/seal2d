local consts = require "consts"
local game = {}

function game.init()
	return require("tests.game.hello_world").entry()
end

function game.pause()
	print('call game.pause()')
end

function game.resume()
	print('call game.resume()')
end

local TOUCH_BEGIN  = consts.TOUCH_BEGIN
local TOUCH_MOVE   = consts.TOUCH_MOVE
local TOUCH_END    = consts.TOUCH_END
local TOUCH_CANCEL = consts.TOUCH_CANCEL

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

function game.event(e)
	if e.type == TOUCH_BEGIN then
		-- print(string.format("begin, (%.2f, %.2f)", e.x, e.y))
	elseif e.type == TOUCH_MOVE then
		-- print(string.format("move, (%.2f, %.2f)", e.x, e.y))
	elseif e.type == TOUCH_END then
		-- print(string.format("end, (%.2f, %.2f)", e.x, e.y))

		-- game.reload()

	elseif e.type == TOUCH_CANCEL then
		-- print(string.format("cancel, (%.2f, %.2f)", e.x, e.y))
	else
		assert(false, "invalid event type = ", e.type)
	end
end

function game.set_root(root)
	game.root = root
end

return game