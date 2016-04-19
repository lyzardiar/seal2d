local engine = require "seal.engine"
local texture = require "seal.texture"
local rect = require "seal.rect"
local util = require "seal.util"

local sdl_render = require "sdl.render"
local sdl_ttf = require "sdl.ttf"
local sdl_enum = require "seal.sdl_enum"

local contains = util.contains

local sprite = {}

local function gc(self)
	if self.frame then
		self.frame.texture:dec_ref()
	end
end

local sprite_meta = {__index = sprite, __gc = gc}

local ZORDER_SHIFT = 16

local SPRITE_TYPE_CONTAINER = 1
local SPRITE_TYPE_IMAGE = 2
local SPRITE_TYPE_TTF = 3
local SPRITE_TYPE_BMFONT = 4

sprite.SPRITE_TYPE_CONTAINER = SPRITE_TYPE_CONTAINER
sprite.SPRITE_TYPE_IMAGE = SPRITE_TYPE_IMAGE
sprite.SPRITE_TYPE_TTF = SPRITE_TYPE_TTF
sprite.SPRITE_TYPE_BMFONT = SPRITE_TYPE_BMFONT

local sprite_id = 0
local function id()
	sprite_id = sprite_id + 1
	return sprite_id 
end

local frames = {}
function sprite.new_sheet(image, atlas)
	local sheet = require(atlas).getSpriteSheetData().frames
	local tex = texture.load(image, "image")
	for k,v in pairs(sheet) do
		local f = {
			name = v.name,
			rect = v.textureRect,
			texture = tex,
		}
		sprite.add_frame(f)
	end
end

function sprite.get_frame(name)
	local f = frames[name]
	if not f then
		assert(false, string.format("sprite.frame, frame not found for name = %s.", name))
	end 
	return f
end

function sprite.add_frame(f)
	if frames[f.name] then
		assert(false, string.format("sprite.add_frame dumplicated frame? name = %s", name))
	end
	frames[f.name] = f
end

local function load_container(self, conf)
	self.width, self.height = conf.width, conf.height
end

local function load_image(self, conf)
	self:set_frame(conf.frame_name)
end

local function load_ttf(self, conf)
	self.__font = sdl_ttf.open(conf.font_path, conf.font_size)
	self:set_text(conf.text or "")
end

local function load_bmfont(self)

end

local function draw_container(self)

end

local function draw_image(self)
	local frame = self.frame
	local rect = frame.rect

	if self.__fast_draw then
		sdl_render.draw(frame.texture.__cobj,
						rect.x,
						rect.y,
						rect.width,
						rect.height,
						self.world_x - self.anchor_x_in_point, 
						WINDOW_HEIGHT - self.world_y - self.height + self.anchor_y_in_point, 
						self.width,
						self.height)
	else
		sdl_render.draw_ex( frame.texture.__cobj,
							rect.x,
							rect.y,
							rect.width,
							rect.height,
							self.world_x - self.anchor_x_in_point, 
							WINDOW_HEIGHT - self.world_y - self.height + self.anchor_y_in_point, 
							self.width * self.world_scale_x, self.height * self.world_scale_y,

							self.world_rotation, 
							self.anchor_x_in_point, self.anchor_y_in_point, 
							self.world_flip)
	end
end

local function draw_ttf(self)
	sdl_render.draw(self.texture.__cobj,
				0,
				0,
				self.width,
				self.height,
				self.world_x - self.anchor_x_in_point, 
				WINDOW_HEIGHT - self.world_y - self.height + self.anchor_y_in_point, 
				self.width,
				self.height)
end

local function draw_bmfont(self)

end

local new_funcs = {
	[SPRITE_TYPE_CONTAINER] = load_container,
	[SPRITE_TYPE_IMAGE] = load_image,
	[SPRITE_TYPE_TTF] = load_ttf,
	[SPRITE_TYPE_BMFONT] = load_bmfont,
}

local draw_funcs = {
	[SPRITE_TYPE_CONTAINER] = draw_container,
	[SPRITE_TYPE_IMAGE] = draw_image,
	[SPRITE_TYPE_TTF] = draw_ttf,
	[SPRITE_TYPE_BMFONT] = draw_bmfont,
}

function sprite.new(type, conf)
	local obj = {
		type = type
	}
	setmetatable(obj, sprite_meta)
	obj:ctor(type, conf)

	return obj
end

function sprite.new_container(conf)
	return sprite.new(SPRITE_TYPE_CONTAINER, conf)
end

-- conf = {
-- frame_name = "seal.png",
-- x = 0, -- optional, default to 0
-- y = 0, -- optional, default to 0
-- }
function sprite.new_image(conf)
	return sprite.new(SPRITE_TYPE_IMAGE, conf)
end

-- conf = {
-- 	text = 'some text',
-- 	font_path = 'res/font/font.ttf',
-- 	font_size = 24,
-- 	x = 0, -- optional, default to 0
-- 	y = 0, -- optional, default to 0
-- }
function sprite.new_ttf(conf)
	return sprite.new(SPRITE_TYPE_TTF, conf)
end

function sprite:ctor(type, conf)
	self.__id = id()
	self.__transform_dirty = true
	self.__fast_draw = true
	self.__sort_child = true
	self.__zorder = 0
	self.__claim_next = true

	self.children = {}
	self.arrive_order = 0
	self.visit_order = {}
	self.parent = nil

	self.anchor_x = 0.5
	self.anchor_y = 0.5
	self.width = 128
	self.height = 128

	-- if the sprite were a image, the frame should be provided.
	self.frame = nil

	-- if this sprite were a label, a text must be provided.
	self.text = nil

	-- the world SRT, for rendering to the final surface
	self.world_x = 0.0
	self.world_y = 0.0
	self.world_scale_x = 1.0
	self.world_scale_y = 1.0
	self.world_rotation = 0.0
	self.world_flip = sdl_enum.SDL_FLIP_NONE -- TODO: flip will influence the children

	-- the local SRT
	self.scale_x = 1.0
	self.scale_y = 1.0
	self.x = conf.x or 0.0
	self.y = conf.y or 0.0
	self.rotation = 0
	self.flip = sdl_enum.SDL_FLIP_NONE

	self.anchor_x_in_point = self.width * self.anchor_x
	self.anchor_y_in_point = self.height * self.anchor_y

	-- touch
	self.touch_enabled = true
	self.touch_swallow = true

	-- actions
	self.actions = {}

	self.type = type
	new_funcs[type](self, conf)
end

function sprite:set_scale(scale_x, scale_y)
	self.scale_x = scale_x ~= nil and scale_x or 1.0
	self.scale_y = scale_y ~= nil and scale_y or 1.0

	self.__fast_draw = false
	self.__transform_dirty = true
end

function sprite:set_rotation(rotation)
	self.rotation = rotation

	self.__fast_draw = false
	self.__transform_dirty = true
end

function sprite:set_pos(x, y)
	self.x, self.y = x, y
	self.__transform_dirty = true
end

function sprite:set_size(width, height)
	self.width, self.height = width, height
end

function sprite:set_anchor(anchor_x, anchor_y)
	self.anchor_x, self.anchor_y = anchor_x, anchor_y
end

function sprite:set_flip(flip)
	self.flip = flip
	self.__transform_dirty = true
end

-- search from frame cache first, use the cache if found.
-- if no frame found, a frame will be created and added to cache
function sprite:set_frame(name)
	local f = frames[name]
	if not f then
		local tex = texture.load(name, "image")
		f = {
			name = name,
			rect = {0, 0, tex.width, tex.height},
			texture = tex,
		}
		sprite.add_frame(f)
	end
	self.frame = f
	self.width, self.height = f.rect.width, f.rect.height

	-- if self.texture then
	-- 	self.texture:dec_ref()
	-- end
	-- self.texture = texture.load(file_name, 'image')
end

function sprite:set_text(text)
	if text and self.text ~= text then
		self.text = text
		if self.texture then
			self.texture:dec_ref()
		end
		self.texture = texture.load(text, 'ttf', self.__font)
		self.width, self.height = self.texture.width, self.texture.height
	end
end

function sprite:set_touch_enabled(enabled)
	self.touch_enabled = enabled
end

function sprite:set_touch_swallow(swallow)
	self.touch_swallow = swallow
end

function sprite:run_action(action)
	self.actions[action] = action
	action:start()
end

function sprite:stop_action(action)
	self.actions[action]:stop()
end

function sprite:stop_all_actions()
	for _, action in pairs(self.actions) do
		action:stop()
	end
 end

function sprite:set_zorder(zorder)
	if zorder ~= self:get_zorder() then
		self.__zorder = zorder << ZORDER_SHIFT + self.arrive_order
		self.__sort_child = true
	end
end

function sprite:get_zorder()
	return self.__zorder >> ZORDER_SHIFT
end

local function sort_child(self)
	local tmp = {}
	for _, child in pairs(self.children) do
		tmp[#tmp+1] = child
	end

	table.sort(tmp, function(a, b)
			return a.__zorder < b.__zorder
		end)
	return tmp
end

function sprite:add_child(child)
	assert(child.parent == nil)
	self.children[child.__id] = child
	child.parent = self

	self.arrive_order = self.arrive_order + 1
	child.__zorder = child.__zorder + self.arrive_order
	self.__sort_child = true
end

function sprite:remove_child(child)
	if self.children[child.__id] then
		child:cleanup()
		self.children[child.__id] = nil
		self.total_children = self.total_children - 1
		self.__sort_child = true
	else
		seal.log(string.format("sprite: child already removed, id = %d", child.__id))
	end
end

function sprite:remove_all_children()
	local keys = {}
	for key, _ in pairs(self.children) do
		keys[#keys+1] = key
	end
	for _, key in ipairs(keys) do
		self.children[key] = nil
	end

	self.children = {}
	self.visit_order = {}
end

function sprite:get_children()
	return self.children
end

function sprite:remove_from_parent()
	assert(self.parent)
	self.parent:remove_child(self)
end

local function update_transform(self)
	local parent = self.parent
	if self.__transform_dirty then
		if parent then
			self.world_x = parent.world_x + self.x
			self.world_y = parent.world_y + self.y

			self.world_scale_x = parent.world_scale_x * self.scale_x
			self.world_scale_y = parent.world_scale_y * self.scale_y

			self.world_rotation = parent.world_rotation + self.rotation
			self.__fast_draw = parent.__fast_draw
		else
			self.world_x = self.x
			self.world_y = self.y

			self.world_scale_x = self.scale_x
			self.world_scale_y = self.scale_y

			self.world_rotation = self.rotation
		end
		
		self.anchor_x_in_point = self.width * self.anchor_x * self.world_scale_x
		self.anchor_y_in_point = self.height * self.anchor_y * self.world_scale_y

		self.__transform_dirty = false
	end
end

function sprite:cleanup()
	-- TODO: do we really need this?
end

function sprite:on_touch(callback)
	self.touch_callback = callback
end

function sprite:perform_touch_event(event)
	if self.touch_callback and self.__claim_next then
		return self.touch_callback(event)
	end
	return false
end

function sprite:update(dt)
	if next(self.actions) then
		for _, action in pairs(self.actions) do
			action:update(dt)
			self.__transform_dirty = true
			self.__fast_draw = false
		end
	end

	for _, child in pairs(self.children) do
		child:update(dt)
	end
end

local function visit_draw(self)
	update_transform(self)

	draw_funcs[self.type](self)
end


local function visit_touch(self, touch_event)
	if self.touch_enabled then
		local aabb = self:aabb_all()
		if contains(aabb, touch_event.x, touch_event.y) and touch_event.type == 'begin' then
			touch_event:add_captured(self)
		end
	end
end

function sprite:__visit(func, ...)
	if self.__sort_child then
		self.visit_order = sort_child(self)
		self.__sort_child = false
	end

	func(self, ...)

	if next(self.children) then
		for _, child in ipairs(self.visit_order) do
			child:__visit(func, ...)
		end
	end
end

function sprite:draw()
	self:__visit(visit_draw)
end

function sprite:touch_test(touch_event)
	self:__visit(visit_touch, touch_event)

	--bubble: now we start to pop with the reverse order
	local captered = touch_event.captered

	for i = #captered, 1 , -1 do
		local spr = captered[i]
		touch_event.current_target = spr
		spr.__claim_next = spr:perform_touch_event(touch_event)
		if touch_event.type == 'end' then
			spr.__claim_next = true
		end
		if spr.touch_swallow then
			break
		end
 	end
end

function sprite:perfrom_touch(event)
	if self.touch_callback then
		self.touch_callback(event)
	end
end

function sprite:aabb_all()
	if next(self.children) then
		local ret = self:aabb_world()
		for _, child in pairs(self.children) do
			ret = rect.merge(ret, child:aabb_world())
		end
		return ret
	else
		return self:aabb_world()
	end
end

function sprite:aabb_world()
	return {
		x = self.world_x - self.anchor_x_in_point, 
		y = WINDOW_HEIGHT - self.world_y - self.height + self.anchor_y_in_point, 
		width = self.width * self.world_scale_x, 
		height = self.height * self.world_scale_y
	}
end

function sprite.__tostring()
	return string.format("{id = %s, x = %d, y = %d}", self.__id, self.x, self.y)
end

return sprite