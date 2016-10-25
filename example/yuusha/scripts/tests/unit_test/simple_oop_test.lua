local fruit = class('fruit')

function fruit:ctor(name)
    self.name = name
end

function fruit:get_type()
    return "fruit"
end

function fruit:get_name()
    return self.name
end

function fruit:set_name(name)
    self.name = name
end

local apple = class('apple', fruit)

function apple:ctor(name)
    apple.__super.ctor(self, name)
end

-- function apple:set_name(name)
--  self.name = name
-- end

function apple:get_type()
    return "apple"
end

local pine_apple = class('pine_apple', apple)

function pine_apple:ctor(name, price)
    self.name = name
    self.price = price
end

function pine_apple:get_price()
    return self.price
end

local luaunit = require "luaunit"
function test_simple_oop()
    local f = fruit.new('sample-fruit')
    f:set_name('name-changed-sample-fruit')

    luaunit.assertEquals(f:get_name(), "name-changed-sample-fruit")

    local a = apple.new('sample-apple')
    luaunit.assertEquals(a:get_name(), "sample-apple")

    a:set_name('name-changed-sample-apple')
    luaunit.assertEquals(a:get_name(), "name-changed-sample-apple")

    local p = pine_apple.new('sample-pine_apple', 5.0)
    
    luaunit.assertEquals(p:get_name(), "sample-pine_apple")
    luaunit.assertEquals(p:get_type(), "apple")
    luaunit.assertEquals(p:get_price(), 5.0)
end

return test_simple_oop