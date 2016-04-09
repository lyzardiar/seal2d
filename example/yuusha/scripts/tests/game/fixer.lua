local fixer = {}

local i = 0

function fixer.inc()
	i = i + 1
	print('after inc, i = ', i)
end

function fixer.print()
	print('i = ', i)
end

return fixer