local function pmain()
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

function main()
	xpcall(pmain, function(err)
			print(err)
			print(debug.traceback("", 2))
		end)
end