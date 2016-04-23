require("lua-hammer.hammer").exports()

local options = {
	"script_path",
	"out",
}

local zip_cmd = "zip"
local find_cmd = "find"
local grep_cmd = "grep"

local function assert_p(t, param)
	if not t[param] then
		assert(false, string.format("--%s is required.", param))
	end
	return t
end

local function main(...)
	-- parse the paths
    local path = assert_p(cmdline.get_opt({...}, "script_path"), "script_path")["script_path"]
    local fullpath
    if path[1] ~= '/' then
    	local pwd = os.capture("pwd")
    	fullpath = string.format("%s/%s", pwd, path)
    else
    	fullpath = path
    end

    -- get all the lua files
    local files = os.capture(string.format("%s %s | grep .lua$", find_cmd, fullpath))
    local file_table = string.split(files, " ")

    -- do some cleanup and ensure the workdir
    os.execute("rm -rf ./tmp && mkdir -p tmp")

    -- start working
    local function last_word_of_path(s)
    	return string.sub(s, string.find(s, "/%w+[/]*$"))
    end

    local last_path = last_word_of_path(path)

    local function parse_filename(path)
    	local _, e = string.find(path, last_path)
		local t = string.sub(path, e+1)
		local s = string.find(t, "^/")
		t = string.sub(t, s+1)
		return  string.sub(path, string.find(path, "[_%w]+.lua$")),
				string.gsub(string.gsub(t, "/", "."), ".lua", "")	-- replace '/' with . and strip .lua
    end

    for _, path in pairs(file_table) do

    	os.execute(string.format("cp %s tmp/", path))

    	local from, to = parse_filename(path)
    	os.execute(string.format("mv tmp/%s tmp/%s",from, to))
    end

    -- pack and cleanup
	os.execute(string.format("cd tmp/ && zip ../code.zip * && cd -"))
	os.execute(string.format("rm -rf tmp"))
end

main(...)