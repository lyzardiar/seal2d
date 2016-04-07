local game = {}
	
function game.start()
	-- start coding here.

	-- define your scene here.
	return require("tests.sprite.hello_world").sprite()
end

function game.pause()
	print('call game.pause()')
end

function game.resume()
	print('call game.resume()')
end

return game