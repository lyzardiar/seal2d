local core = require "sprite_core"
local sprite_frame = require "seal.sprite_frame"

local sprite = {
    get_frame_from_cache = core.get_frame_from_cache,
    load_sprite_frame = core.load_sprite_frame,
    unload_sprite_frame = core.unload_sprite_frame,
    set_frame_texture_id = core.set_frame_texture_id,
    set_text = core.set_text,
    register_handler = core.register_handler,
    clean_handler = core.clean_handler,
    run_action = core.run_action,
    set_anim = core.set_anim,
    set_spine_anim = core.set_spine_anim,
    set_anim_interval = core.set_anim_interval,
    set_visible = core.set_visible,
    set_pos = core.set_pos,
    set_anchor = core.set_anchor,
    set_rotation = core.set_rotation,
    set_scale = core.set_scale,
    set_color = core.set_color,
    set_size = core.set_size,
    get_pos = core.get_pos,
    get_size = core.get_size,
    get_anchor = core.get_anchor,
    get_glyph = core.get_glyph,
    add_child = core.add_child,
    remove_from_parent = core.remove_from_parent,
    remove_all_child = core.remove_all_child
}

local meta = {__index = sprite}

function sprite.new(...)
    local self = {}
    setmetatable(self, meta)
    local __frame = sprite_frame.get(...)
    self.__cobj = core.new(__frame)
    return self
end

function sprite.new_container(...)
    local self = {}
    setmetatable(self, meta)
    self.__cobj = core.new_container(...)
    return self
end

function sprite.new_bmfont_label(label, path, line_width)
    local self = {}
    setmetatable(self, meta)
    self.__cobj = core.new_bmfont_label(label, path, line_width or 0)
    return self
end

function sprite.new_spine(atlas_file, spine_data_file, scale)
    local self = {}
    setmetatable(self, meta)
    self.__cobj = core.new_spine(atlas_file, spine_data_file, scale or 1.0)
    return self
end

function sprite.new_primitive(...)
    local self = {}
    setmetatable(self, meta)
    self.__cobj = core.new_primitive(...)
    return self
end

function sprite.new_clip(...)
    local self = {}
    setmetatable(self, meta)
    self.__cobj = core.new_clip(...)
    return self
end

function sprite.new_scale9(texture_name, frame_name, rect)
    local self = {}
    setmetatable(self, meta)
    local frame = sprite_frame.get(texture_name, frame_name)
    local r = rect
    if not r then -- estimate the rect if not provied.
        local w, h = core.get_sprite_frame_size(frame)
        r = {x = w/2, y = h/2, w = w/10, h = h/10}
    end
    self.__cobj = core.new_scale9(frame, r)
    return self
end

function sprite:cleanup(...)
    self:remove_all_child(...)
    self:clean_handler()
end

function sprite:set_bbox_visible(visible)
    if not self.__bbox_lines then

        local ax, ay = self:get_anchor()
        local w, h = self:get_size()
        local xoffset = -w * (ax)
        local yoffset = -h * (ay)

        --[[
           p3-----p2
            |     |
            |     |
           p0_____p1
        ]]
        local x0, y0 = 0 + xoffset, 0 + yoffset
        local x1, y1 = w + xoffset, 0 + yoffset
        local x2, y2 = w + xoffset, h + yoffset
        local x3, y3 = 0 + xoffset, h + yoffset

        local lines = {
            {x0, y0, x1, y1},
            {x1, y1, x2, y2},
            {x2, y2, x3, y3},
            {x3, y3, x0, y0}
        }

        local bbox_lines = {}
        for i = 1, #lines do
            local line = sprite.new_primitive("L", {
                                            vertex = lines[i],
                                            width = 2.0,
                                            color = {30, 0, 0, 255}} )
            self:add_child(line)
            bbox_lines[#bbox_lines+1] = line
        end
        self.__bbox_lines = bbox_lines
    end

    for _, line in pairs(self.__bbox_lines) do
        line:set_visible(visible)
    end
end

return sprite