local tween = require "seal.tween"
local action = {}

local action_meta = {__index = action}

local function new_base()
	return {
		__state = 'stopped'
	}
end


-- loop: if loop < 0  -> infinite loop
--		 if loop == 0 -> don't loop, only run once
--		 if loop > 0  -> run $loop times, 
--						 so if loop = 0 or loop = 1 will make the action run at least once

--- ... : this is parameters for tween, so checkout tween for details
function action.new_tween_loop(loop, ...)
	local obj = new_base()
	obj.tween = tween.new(...)
	obj.time_scale = 1.0

	if not loop or loop == 0 then
		obj.loop = 1
	else
		obj.loop = loop
	end

	setmetatable(obj, action_meta)

	return obj
end

function action.tween(...)
	return action.new_tween_loop(1, ...)
end

function action.call(call)
	assert(call and type(call) == 'function', 
			string.format('function expected, got %s.', type(call)))

	local obj = new_base()
	obj.call = call

	setmetatable(obj, action_meta)
	obj.update = function(dt)
		obj.call()
		obj:stop()
		return true
	end
	return obj
end

local function queue_start(self)
	self.__state = 'running'
	self.current:start()
end

local function queue_update(self, dt)
	if not self:stopped() then
		if self.current:stopped() then
			local current = self:pop()
			if current then
				current:start()
			end
			self.current = current
		end

		-- if there's no new action in queue, then the whole action_queue is stopped
		if not self.current then
			self.__state = 'stopped'
			return true
		else
			self.current:update(dt)
			return false
		end
	end
end

local function queue_pop(self)
	local index = self.index + 1
	if index <= #self.actions then
		self.index = index
		return self.actions[index]
	else
		return nil
	end
end

function action.queue(...)
	local actions = {...}
	assert(#actions > 0, "no action passed to action_queue when init.")

	local obj = new_base()
	obj.actions = actions

	obj.index = 1
	obj.current = obj.actions[obj.index]
	
	setmetatable(obj, action_meta)

	obj.start = queue_start
	obj.update = queue_update
	obj.pop = queue_pop	

	return obj
end

function action:set_timescale(scale)
	self.time_scale = scale
end

function action:update(dt)
	if self.__state == 'running' then
		local fin = self.tween:update(dt * self.time_scale)
		if fin then
			if self.loop < 0 then
				self.tween:reset()
			elseif self.loop == 1 then
				self.__state = 'stopped'
			else
				self.loop = self.loop - 1
				self.tween:reset()
			end
		end
	elseif self.__state == 'paused' then
		-- pass, do no thing
	elseif self.__state == 'stopped' then
		-- do nithing
	end
end

function action:start()
	if self.__state == 'stopped' then
		self.__state = 'running'
	else
		assert(false, 'try to start in invalid state = ' .. self.__state)
	end
end

function action:pause()
	if self.__state == 'running' then
		self.__state = 'paused'
	else
		assert(false, 'try to pause in invalid state = ' .. self.__state)
	end
end

function action:resume()
	if self.__state == 'paused' then
		self.__state = 'running'
	else
		assert(false, 'try to resume in invalid state = ' .. self.__state)
	end
end

function action:stop()
	if self.__state == 'running' or self.__state == 'paused' then
		self.__state = 'stopped'
	end
end

function action:stopped()
	return self.__state == 'stopped'
end

return action