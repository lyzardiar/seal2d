local fruit = class('fruit')

function fruit:ctor(name)
	print('call fruit:ctor')
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
	print('call apple ctor')
	print_r(apple)
	print('aplle super is')
	print_r(apple.__super)
	apple.__super.ctor(self, name)
end

-- function apple:set_name(name)
-- 	self.name = name
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

local function test()
	print_r(fruit)
	local f = fruit.new('sample-fruit')
	print_r(f)
	print_r(getmetatable(f))
	print(f:get_name())

	f:set_name('name-changed-sample-fruit')
	print(f:get_name())

	local a = apple.new('sample-apple')
	print(a:get_name())
	a:set_name('name-changed-sample-apple')
	print(a:get_name())

	print(a:get_type())

	local p = pine_apple.new('sample-pine_apple', 5.0)
	print("name of p = ", p:get_name())
	print("type of p = ", p:get_type())
	print("price = ", p:get_price())
end

return test