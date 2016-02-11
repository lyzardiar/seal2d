local engine = require "seal.engine"

local sdl_texture = require "sdl.texture"
local sdl_ttf = require "sdl.ttf"
local texture = {}

-- cache stuff
local hit = 0
local miss = 0
local total_entry = 0
local total_size = 0

local max_pool_size = 256
local max_mem_size = 1024*1024*1024

local cache = {}

local function gc(self)
	if self.type == 'image' then
		print(string.format('texture free name = %s', self.name))
		total_entry = total_entry - 1
		total_size = total_size - self.__mem_size
		sdl_texture.free(self.__cobj)
	elseif self.type == 'ttf' then
		print('free ttf texture. text = ', self.text)
		sdl_texture.free(self.__cobj)
	else

	end
end

local texture_meta = {__index = texture, __gc = gc}

function texture.new(name, type)
	local obj = {
		name = name,
		type = type,
		__visit = 1,
		ref = 1,
	}
	setmetatable(obj, texture_meta)
	return obj
end

local function load_font_texture(self, text, fd)
	self.__cobj, self.width, self.height = sdl_ttf.load_texture(fd, text)
	self.text = text
end

function texture.load(name, type, ...)
	if type == 'image' then
		local t = cache[name]
		if t then
			hit = hit + 1
			t.__visit = t.__visit + 1
		else
			if total_entry > max_pool_size or total_size > max_mem_size then
				local tmp = {}
				for _, texture in pairs(cache) do
					if texture.ref == 0 then
						tmp[#tmp+1] = texture
					end
				end
				table.sort(tmp, function(a, b)
						if a.__visit == b.__visit then
							return a.__mem_size < b.__mem_size
						end
						return a.__visit < b.__visit
					end)

				for i = 1, #tmp/4 do
					cache[tmp[i]] = nil
				end
				collectgarbage()
			end

			miss = miss + 1
			total_entry = total_entry + 1
			t = texture.new(name)
			t.__cobj, t.width, t.height = sdl_texture.load(name, engine.gl_render)
			t.__mem_size = t.width * t.height
			cache[name] = t
		end
		return t
	elseif type == 'ttf' then
		-- ttf label texture will not be cached
		local t = texture.new(name, type)
		load_font_texture(t, name, ...)
		return t
	else

	end
end

function texture:dec_ref()
	self.ref = self.ref - 1
	if self.ref == 0 then
		self = nil
	end
end

function texture.clean_cache()
	local tmp = {}
	for texture_name, texture in pairs(cache) do
		if texture.ref == 0 then
			tmp[#tmp+1] = texture_name
		end
	end
	for i = 1, #tmp do
		cache[tmp[i]] = nil
	end
	collectgarbage()
end

function texture.cache_statics()
	print(string.format("hit = %d, miss = %d, rate = %.2f", hit, miss, 
		(hit == 0 or miss == 0) and 0 or (hit)/(hit+miss) ))
end

return texture