local sample_common = {}

local function load_sprite(texture, data, x, y)
    local s = sprite.new(texture, data.frame)
    s:set_pos(x, y)

    s:set_scale(data.scale or 1.0)

    if data.rotation then
        s:set_rotation(data.rotation)
    end

    if data.anchor then
        s:set_anchor(data.anchor.x, data.anchor.y)
    end

    if data.pos then
        s:set_pos(data.pos.x, data.pos.y)
    end

    if data.test_description then
        s.test_description = data.test_description
    end
    return s
end

function sample_common.load_skeltons(parent, texture, sprites, x0, y0)
    local child = {}
    local x, y = x0 or 0, y0 or 0
    for i = 1, #sprites do
        s = load_sprite(texture, sprites[i], x + 100, y)
        x = x + s:get_size()
        parent:add_child(s)
        child[#child+1] = s
    end
    return child
end

return sample_common