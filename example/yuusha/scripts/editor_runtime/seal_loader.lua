local cjson = require "cjson"
local sprite = require "seal.sprite"
local platform_core = require "platform_core"

local seal_loader = {}

function seal_loader.config()

end

local function set_geometry(sprite, opt)
	local size = opt.size
	if size then
		sprite:set_size(size.width, size.height)
	end

	local ax, ay = 0.5, 0.5
	if opt.anchor then
		ax, ay = opt.anchor.x, opt.anchor.y
	end
	sprite:set_anchor(ax, ay)

	local x, y = 0, 0
	if opt.pos then
		x, y = opt.pos.x, opt.pos.y
	end
	sprite:set_pos(x, y)
end

local function set_frame(sprite, frame)

end

local function load_container(opt)
	assert(opt.type == 'container')
	local container = sprite.new_container()
	set_geometry(container, opt)
	return container
end

local function load_sprite(opt)
	local s = sprite.new(opt.frame.atlas, opt.frame.name)
	set_geometry(s, opt)
	return s
end

local loaders = {
	container = load_container,
	sprite 	  = load_sprite,
}

local function __load(opt)
	local load_func = loaders[opt.type]
	if not load_func then
		error(string.format("unsupported type: %s", opt.type))
	end

	local s = load_func(opt)
	local children = opt.children
	if children then
		for i, c in ipairs(children) do
			local child = __load(c)
			s:add_child(child)
		end
	end
	return s
end

function seal_loader.load(file)
	local data = platform_core.read_s(file)
	local parsed = cjson.decode(data)
	local root = __load(parsed.scene)

	return root
end

return seal_loader