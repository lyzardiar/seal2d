local sprite_frame = require "seal.sprite_frame"
local sprite = require "seal.sprite"
local nuk_core = require "nuklear_core"

local main_panel = {}

local menu_panel = require("editor.menu_panel")
local title_panel = require("editor.title_panel")
local anim_panel
local prop_panel

function main_panel.draw()
    nuk_core.nuk_draw_start()

    menu_panel.draw()
    title_panel.draw()

    nuk_core.nuk_draw_end()
end

function main_panel.load()
    menu_panel.load()
    title_panel.load()
end

return main_panel