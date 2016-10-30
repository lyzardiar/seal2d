local nuk_node = require "seal.nuk_node"
local layout = require("editor.editor_layout").title_panel

local title_panel = {}

local render
local title_name = ""

function title_panel.load()
    render = nuk_node.new()
end

function title_panel.set_title_name(name)
    title_name = name
end

function title_panel.draw()
    if render:nk_begin(title_name, { x = 0, y = 0,
                                 w = WINDOW_WIDTH,
                                 h = layout.height
                                }, nuk_node.NK_WINDOW_TITLE) then

    end

    render:nk_end()
end

return title_panel