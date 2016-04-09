local test = {}

local socket = require "socket.core"
local http = require "socket.http"

http.TIMEOUT = 10

local function request(url)
	local start = socket.gettime()
	print('start = ', os.time())
	local result = http.request(url)
	if result then
		print("result len = ", #result)
		print('end = ', os.time())
		print("time cost = ", socket.gettime() - start)
		print(#result > 0 and "socket" or "test failed.")
		coroutine.yield()
	else
		print("no result?")
	end
end

function test.sprite()
	print("--- start ---")
	-- local co = coroutine.create(request)
	-- coroutine.resume(co, "http://spell.darkzore.com")

	print("--- finished ---")

end


return test