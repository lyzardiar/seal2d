local rect = {}

local function max_x(r)
	return r.x + r.width
end

local function max_y(r)
	return r.y + r.height
end

local function min_x(r)
	return r.x
end

local function min_y(r)
	return r.y
end

function rect.merge(r1, r2)
	local x = math.min(min_x(r1), min_x(r2))
    local y = math.min(min_y(r1), min_y(r2))
    local width = math.max(max_x(r1), max_x(r2)) - x
    local height = math.max(max_y(r1), max_y(r2)) - y
    return {x = x, y = y, width = width, height = height}
end

return rect