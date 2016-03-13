local hello = {}

local fixer = require "tests.sprite.fixer"
function hello.sprite()
	
	fixer.inc()

	fixer.print()
end

function hello.reload()
	for k,v in pairs(_G) do
		print(k, v)
	end
end

return hello