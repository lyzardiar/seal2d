local lxp = require "lxp"
local platform = require "platform_core"
local cjson = require "cjson"

local score = {}

function score.new(json_path)
    local data = cjson.decode(platform.read_s(json_path))
    local self = {}
    setmetatable(self, {__index = score})
    self.__data = data

    -- cache something for easier use, it's readonly, would cause no side effect.
    self.page_width = self.__data["score-partwise"]["defaults"]["page-layout"]["page-width"]["$t"]
    return self
end

local measure_attrs = { ["width"] = true, ["number"] = true }
local function draw_measure(self, measure)
    local measure_width = measure.width

    local measure_cursor_x, measure_cursor_y = 0, 0

    local function element_print_draw(self, _print, measure_width)
    -- for the sake of global print funciton, we add an underscore to _print.
        local page_width = self.page_width
        local distance =
            (_print["system-layout"]["top-system-distance"] or _print["system-layout"]["system-distance"])["$t"]

        measure_cursor_y = measure_cursor_y + distance

        -- here we go draw the print
        -- print("page_width, distance = ", page_width, distance)
    end

    local element_draw_func = {
        ["print"] = element_print_draw,
    }

    for element, data in pairs(measure) do
        if not measure_attrs[element] then
            -- becasure our data structure merge the attrs and elements,
            -- so we need test if this is really a element.

            local draw = element_draw_func[element]
            if draw then
                draw(self, data, measure_width)
            else
                -- printf("element: %s not supported yet.", element)
            end
        end
    end
end

local function draw_part(self, part)
    local measures = part.measure

    for i, data in pairs(measures) do
        draw_measure(self, data)
    end
end

function score:draw()
    local part = self.__data["score-partwise"]["part"]
    draw_part(self, part)
end

return score