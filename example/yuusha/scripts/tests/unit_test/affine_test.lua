-- not used yet.
local function test()
	local affine = require "seal.affine"
	local a = affine.alloc()
	affine.translate(a, 10, 20)

	local t = affine.serial_t(a)
	for k,v in pairs(t) do
		print(k,v)
	end
end

return test