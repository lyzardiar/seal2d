local part = class("part")

function part.new(conf)
	local self = conf or {}
	self.measures = {}

	return setmetatable(self, {__index = part})
end

function part:add_measure(m)
	self.measures[#self.measures+1] = m
end

function part:draw()
	for i, m in ipairs(self.measures) do
		m:draw()
	end
end

return part