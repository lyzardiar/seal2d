local timer = {}

local all_timers = {}

function timer.new(conf)
	local obj = conf or {
		interval = 0,
		callback = function() end,
		loop = 0,
	}
	obj.loop = obj.loop or 1
	obj.now = 0
	obj.stopped = false
	all_timers[obj] = obj
	return setmetatable(obj, {__index = timer})
end

function timer.update(dt)
	local stopped_timers = {}
	for k,v in pairs(all_timers) do
		if v:step(dt) then
			stopped_timers[#stopped_timers+1] = k
		end
	end
	for i = 1, #stopped_timers do
		all_timers[stopped_timers[i]] = nil
	end
end

function timer:step(dt)
	local next_dt = self.now + dt

	if self.stopped then
		return true
	end

	if self.loop < 0 then
		if next_dt > self.interval then
			self.callback()
			self.now = 0
		else
			self.now = next_dt
		end
	else
		if next_dt > self.interval then
			self.loop = self.loop - 1
			if self.loop == 0 then
				self.stopped = true
			end
			self.callback()
			self.now = 0
		else
			self.now = next_dt
		end
	end
	
	return false
end

return timer