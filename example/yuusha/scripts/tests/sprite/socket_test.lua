local test = {}

local socket = require "socket.core"
local http = require "socket.http"

http.TIMEOUT = 2
function test.sprite()
	local start = socket.gettime()
	print('start = ', os.time())
	local result = http.request("http://spell.darkzore.com")
	print("result = ", result)
	print('end = ', os.time())
	print("time cost = ", socket.gettime() - start)
	print(#result > 0 and "socket" or "test failed.")
end


return test