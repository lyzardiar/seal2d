local util = {}

function util.class(name, super)
    local __class
    if type(super) == 'function' then
        __class = super()
    elseif type(super) == 'table' then
        __class = super
    else
        assert(type(self) == nil)
    end

    __class.new = function(...)
        local obj = {}
        setmetatable(obj, {__index = __class})
        obj:ctor(...)
        return obj
    end

    return __class
end


local log_level_tag = {
    [1] = 'D',
    [2] = 'I',
    [3] = 'W',
    [4] = 'E',
 }

function util.log(level, ...)
    print(log_level_tag .. '\t', ...)
end


function util.print_r(root)
    if not root then
        print("attempt to print an empty table.")
        print(debug.traceback("", 2))
        return
    end
    if type(root) ~= "table" then
        print(string.format("attempt to print a table, but it's type = %s, value = %s.",
                type(root), tostring(root)))
        return
    end

    local cache = { [root] = "." }
    local function _dump(t,space,name)
          local temp = {}
          for k,v in pairs(t) do
              local key = tostring(k)
              if cache[v] then
                  table.insert(temp,"+" .. key .. " {" .. cache[v].."}")
              elseif type(v) == "table" then
                  local new_key = name .. "." .. key
                  cache[v] = new_key
                  table.insert(temp,"+" .. key .. _dump(v,space .. (next(t,k) and "|" or " " )
                                    .. string.rep(" ",#key),new_key))
              else
                  table.insert(temp,"+" .. key .. " [" .. tostring(v).."]")
              end
          end
          return table.concat(temp,"\n"..space)
    end

    local info =  "\n------------------------------------------------------------------------\n" ..
                    _dump(root, "","") ..
                  "\n------------------------------------------------------------------------\n"
    io.write(info)
    io.flush()
end

function util.contains(rect, x, y)
    return  x >= rect.x and
            x <= rect.x + rect.width and
            y >= rect.y and
            y <= rect.y + rect.height
end

function util.nkeys(t)
    local total = 0
    for k,v in pairs(t) do
        total = total + 1
    end
    return total
end

function util.printf(...)
    print(string.format(...))
end

function util.tobytes(s)
    local t = {}
    local tinsert = table.insert
    for i = 1, #s do
        tinsert(t, string.byte(s, i))
    end
    return table.concat(t, " ")
end

function util.shuffle_t(t)
    return util.shuffle(#t)
end

function util.shuffle(total)
    assert(total)
    local base = {}
    local results = {}
    for i=1, total do
        base[i] = i
    end
    for i=1, total do
        local k = math.random(i, total)
        base[i], base[k] =  base[k],  base[i]
    end
    for i=1, total do
        results[i] = base[i]
    end
    return results
end

function util.order_pairs(t)
    local tinsert = table.insert

    local ot = {}
    -- sort the keys
    for k, _ in pairs(t) do
        tinsert(ot, k)
    end

    table.sort(ot, function(a, b)
            return tostring(a) < tostring(b)
        end)

    local index = 0
    local function iter()
        index = index + 1
        if t[ot[index]] then
            return index, t[ot[index]]
        end
    end

    return iter, t, nil
end

function util.shuffle_pairs(t)
    local tinsert = table.insert
    local index = 0
    local ot = {}
    -- sort the keys
    for k, _ in pairs(t) do
        tinsert(ot, k)
    end

    local shuffle_t = util.shuffle_t(ot)

    local index = 0
    local function iter()
        index = index + 1
        if t[shuffle_t[index]] then
            return index, t[shuffle_t[index]]
        end
    end

    return iter, t, nil
end

return util