local note = {}

function note.new(conf)
	local self = conf or {}
	return setmetatable(self, {__index = note})
end

function note:draw()

end

return note