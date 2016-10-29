local sprite = require "seal.sprite"
local sprite_frame = require "seal.sprite_frame"
local nuk_node = require "seal.nuk_node"

local stage = class("stage", function()
		return sprite.new_container()
	end)

function stage:ctor()
	sprite_frame.load_from_json("res/images/ui.json")
    sprite_frame.load_from_json("res/images/anim_pirate.json")
    sprite_frame.load_from_json("res/images/skeleton.json")

    if device.is_pc() then
        self.menu = nuk_node.new()
    end
end

function stage:switch(new)
	print("#1 call switch to new", new)
    if self.current ~= new then
    	print("#2 not equal")
        if self.current and self.current.on_exit then
            self.current:on_exit()
        end

        if self.current then -- current may not exist for the first time.
        	print("#3 remove current")
            self.current:remove_from_parent()
        end

        self:add_child(new)
        print("#4 add new")
        if new.on_enter then
            new:on_enter()
        end
    	self.current = new
    	print("#5 replace fin")
    end
end

local tests = {
    { name = "hello world", sameple_name = "hello_world" },
    { name = "sprite srt test", sameple_name = "sprite_srt" },
}

local function draw_menu(self)
    local menu = self.menu

    if (menu:nk_begin("tests",
        {x = WINDOW_WIDTH - 200, y = 50, w = 200, h = WINDOW_HEIGHT - 200/2},
            nuk_node.NK_WINDOW_BORDER      |
            nuk_node.NK_WINDOW_MOVABLE     |
            nuk_node.NK_WINDOW_SCALABLE    |
            nuk_node.NK_WINDOW_MINIMIZABLE |
            nuk_node.NK_WINDOW_TITLE
        )) then

        -- the layout size should never be larger than the panel size.....
        menu.nk_layout_row_static(30, 150, 1)

        for i = 1, #tests do
            local t = tests[i]
            if (menu.nk_button_label(t.name, menu.NK_BUTTON_DEFAULT)) then
            	local node = require("tests.game.samples." .. t.sameple_name)
                self:switch(node.new())
            end
        end
    end
    menu.nk_end()
end

function stage:draw()
	if device.is_pc() then
		nuk_node.draw_start()
    	draw_menu(self)
    	nuk_node.draw_end()
    end

end

return stage