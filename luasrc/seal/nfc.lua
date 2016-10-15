local nfc = {}

local listeners = {}

function nfc.register(event, callback)
	assert(event and type(event) == 'string')
	assert(callback and type(callback) == 'function')

	local t = listeners[event]
	if not t then
		t = {}
		listeners[event] = t
	end

	t[#t+1] = callback
end

function nfc.register_events(events)
	assert(type(events) == 'table')
	for _, e in ipairs(events) do
		nfc.register(e.event_name, e.callback)
	end
end

function nfc.remove(event, callback)
	local t = listeners[event]
	if not t then
		assert(false, string.format("not found listener for event = %s", event))
		return
	end

	local found
	for k,v in pairs(t) do
		if v == callback then
			found = k
		end
	end
	if found then
		t[found] = nil
		if not next(t) then
			print(string.format("All listener of [%s] has been removed", event))
			listeners[event] = nil
		end
	else
		print('**** Warning, not found callback')
	end
end

function nfc.remove_event_name(event_name)
	if listeners[event_name] then
		listeners[event_name] = nil
	else
		error(string.format('not found listener name = %s', event_name))
	end
end

function nfc.remove_events(events)
	assert(type(events) == 'table')
	for _, e in pairs(events) do
		nfc.remove(e.event_name, e.callback)
	end
end

function nfc.dispatch(event, ...)
	local events = listeners[event]

	if not events then
		return
	end

	if not next(listeners[event]) then
		error(string.format("no event[%s] listener to dispatch", event))
		return
	end

	for _, callback in pairs(events) do
		assert(callback)
		callback(...)
	end
end

-- call this when restart the game.
function nfc.cleanup()
	listeners = {}
end

return nfc