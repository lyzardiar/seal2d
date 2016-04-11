local game = {}

local timer = require "timer"	
function game.init()
	-- start coding here.

	-- define your scene here.
	print("call game.init")

	local t = timer.new { 
				interval = 1, 
				callback = function() 
					print("call timer") 
					os.exit(0)
				end}

	return require("tests.game.hello_world").entry()
end

function game.pause()
	print('call game.pause()')
end

function game.resume()
	print('call game.resume()')
end

return game