local nuk_node = require "seal.nuk_node"
local layout = require("editor.editor_layout").menu_panel
local title_layout = require("editor.editor_layout").title_panel

local menu_panel = {}


local render

function menu_panel.load()
    render = nuk_node.new()
end

local function on_new_project()
    print("call on_new_project")
end

local function on_open_project()
    print("call on_open_project")
end

local function on_save_project()
    print("call on_save_project")
end

local function on_new_story()
    print("call on_new_story")
end

local items = {
    { name = "new project", on_click = on_new_project },
    { name = "open project", on_click = on_open_project },
    { name = "save project", on_click = on_save_project },
    { name = "new story", on_click = on_new_story },
}

function menu_panel.draw()
    if render:nk_begin("menu", { x = 0, y = title_layout.height,
                                 w = layout.width + layout.padding,
                                 h = layout.height
                                }) then
        render.nk_layout_row_static(layout.row_height,
                                    layout.width, 1)
        for i, item in ipairs(items) do
            if (render.nk_button_label(item.name, nuk_node.NK_BUTTON_DEFAULT)) then
                item.on_click()
            end
        end
    end

    render:nk_end()
end

return menu_panel