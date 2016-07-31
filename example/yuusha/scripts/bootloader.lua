local function pmain()
	local platform = require("platform_core")
	local script_search_path = {
		"scripts/?.lua",
		"scripts/seal/?.lua",
		"scripts/seal/thirdparty/socket/?.lua",
		"scripts/seal/thirdparty/cov/?.lua;",
		"scripts/seal/thirdparty/luaunit/?.lua;",
		"res/?.lua",
	}
	
	local plat = platform.get_platform()
	if plat == 'mac' then
		for _, path in ipairs(script_search_path) do
			package.path = package.path .. ";" .. path
		end

	elseif plat == 'ios' then
		local root = platform.get_sandbox_root_path()
		for _, path in ipairs(script_search_path) do
			package.path = package.path .. ";" .. root .. "/" .. path
		end
	else
		assert(false, "other platform should write the load function.")
	end


	print("package.path = ", package.path)
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