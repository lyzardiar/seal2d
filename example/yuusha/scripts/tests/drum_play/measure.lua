local measure = {}

function measure.new(conf)
	local self = conf or {}
	self.notes = {}

	return setmetatable(self, {__index = measure})
end

function measure:add_note(note)
	self.notes[#self.notes+1] = noted
end

return measure