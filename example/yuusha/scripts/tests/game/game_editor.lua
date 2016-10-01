local sprite = require "seal.sprite"

local game_editor = {}

local root
local main_panel = require "editor.main_panel"

function game_editor.on_reload()

end

function game_editor.draw()
	main_panel.draw()
end

function game_editor.entry()
	root = sprite.new_container()

	main_panel.load()
	return root.__cobj
end

return game_editor