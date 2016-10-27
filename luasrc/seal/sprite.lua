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

function sprite.new_bmfont_label(...)
    local self = {}
    setmetatable(self, meta)
    self.__cobj = core.new_bmfont_label(...)
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

function sprite:cleanup(...)
    self:remove_all_child(...)
    self:clean_handler()
end

function sprite:set_bbox_visible(visible)
    if not self.__bbox_lines then
        local g = self:get_glyph()

        local ax, ay = self:get_anchor()
        local w, h = self:get_size()
        local xoffset = w * (ax)
        local yoffset = h * (ay)

        local lines = {
            {g[1] - xoffset, g[2] - yoffset, g[3] - xoffset, g[4] - yoffset},
            {g[3] - xoffset, g[4] - yoffset, g[5] - xoffset, g[6] - yoffset},
            {g[5] - xoffset, g[6] - yoffset, g[7] - xoffset, g[8] - yoffset},
            {g[7] - xoffset, g[8] - yoffset, g[1] - xoffset, g[2] - yoffset},
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