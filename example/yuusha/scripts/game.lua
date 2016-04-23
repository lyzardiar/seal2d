local consts = require "seal.consts"

local game = {}

local function export_module()
	local util = require "seal.util"
	local modules = {
				util = util,
			}

	for mod_name, mod in pairs(modules) do
		for k, v in pairs(mod) do
			_G[k] = v
		end
	end
end

function game.init()
	export_module()
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
	print("start reload The loaded packages are")
	local loaded = {}
	for k,v in pairs(package.loaded) do
		print(k)
		loaded[#loaded+1] = k
	end

	for k,v in pairs(loaded) do
		package.loaded[v] = nil
	end


	require("engine_core").reload_script()

	print("reload finished.")

	local hello_world = require("tests.game.hello_world")
	print("helloworld = ", hello_world)
	hello_world.print_hello()
end

function game.event(e)
	if e.type == TOUCH_BEGIN then
		print(string.format("begin, (%.2f, %.2f)", e.x, e.y))
	elseif e.type == TOUCH_MOVE then
		print(string.format("move, (%.2f, %.2f)", e.x, e.y))
	elseif e.type == TOUCH_END then
		print(string.format("end, (%.2f, %.2f)", e.x, e.y))

		game.reload()

	elseif e.type == TOUCH_CANCEL then
		print(string.format("cancel, (%.2f, %.2f)", e.x, e.y))
	else
		assert(false, "invalid event type = ", e.type)
	end
end

function game.set_root(root)
	game.root = root
end

return game