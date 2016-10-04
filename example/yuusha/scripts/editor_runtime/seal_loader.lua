local cjson = require "cjson"

local seal_loader = {}

function seal_loader.config()

end

local function load_container(opt)

end

local function load_sprite(opt)

end

local loaders = {
	container = load_container,
	sprite 	  = load_sprite,
}

local function load_scene(opt)
	local load_func = loaders[opt.type]
	if not load_func then
		error(string.format("unsupported type: %s", opt.type))
	end

end

function seal_loader.load(data)
	local parsed = cjson.decode(data)
	local root = load_scene(parsed.scene)

	return root
end

return seal_loader