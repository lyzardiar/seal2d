local stm = {}

local stm = {}
function stm.new(conf)
	local obj = {}
	setmetatable(obj, {__index = stm})
	obj:ctor(conf)
	return obj
end

function stm:ctor(conf)
	self.init = conf.init
	self.events = conf.events
	self.callbacks = conf.callbacks

	assert(self.init and self.events)

	self.current = self.init

	local map = {}
	for k,v in pairs(conf.events)do
		map[v.name] = v
	end
	self.map = map

	local function transition(name)
		return function(self, ...)
			local ok, to = self:can_do_event(name)
			if ok then
				local from = self.current
				self.current = to
				local callback_name = "on_" .. name
				local callback = self.callbacks[callback_name]
				if callback then
					callback(self, from, to, ...)
				else
					assert(false)
					-- event with no callback is valid here. do nothing
				end
			else
				assert(false, string.format("can not do event %s at state %s", name, self.current))
			end
		end
	end

	for _, v in pairs(self.events) do
		local name = v.name
		self[name] = transition(name)
	end
end

function stm:can_do_event(name)
	local event = self.map[name]
	assert(event, string.format("unknown event name %s", name))
	return event.from == self.current, event.to
end

function stm:is(state)
	return self.current == state
end


return stm