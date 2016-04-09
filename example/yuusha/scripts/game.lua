local game = {}
	
function game.init()
	-- start coding here.

	-- define your scene here.
	print("call game.init")
	return require("tests.game.hello_world").entry()
end

function game.pause()
	print('call game.pause()')
end

function game.resume()
	print('call game.resume()')
end

return game