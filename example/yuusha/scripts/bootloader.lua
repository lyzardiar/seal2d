-- this function will inject some functions into _G
-- if you don't like this style, just comment this function
local bootloader = {}

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
	
	export_module()

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