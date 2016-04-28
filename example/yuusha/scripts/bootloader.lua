-- this function will inject some functions into _G
-- if you don't like this style, just comment this function
local bootloader = {}

local function main()
	package.path = package.path .. 
	";scripts/?.lua" ..
	";scripts/seal/?.lua" ..
	";scripts/seal/thirdparty/socket/?.lua" ..
	";scripts/seal/thirdparty/cov/?.lua" .. 
	";scripts/seal/thirdparty/luaunit/?.lua" ..
	";res/?.lua"

	-- require "luacov"
	-- require "luacov.tick"

	local game = require "game"
	require("seal.engine").start(game)
end

function bootloader.start()
	xpcall(main, function(err)
			print(err)
			print(debug.traceback("", 2))
		end)
end

return bootloader