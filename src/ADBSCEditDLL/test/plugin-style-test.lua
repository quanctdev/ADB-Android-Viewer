
 -- Android ADB Viewer 0.0.4.25 - 30/10/2019
 -- Display resolution: 1024x600
 -- Date build script: Wed Oct 30 03:12:52 2019


function main (stateOld)
	local tbl_screen01 = {
		{1431702,127,139,143},
		{1434774,127,139,143},
		{1440930,127,139,143},
		{1444002,127,139,143},
		{1274736,77,182,172},
		{1262451,77,182,172},
		{1265523,77,182,172},
		{1649526,91,105,111},
		{1637241,224,228,230},
		{1640313,67,82,90},
		{1643385,57,72,80},
		{1646457,55,71,79},
		{1649529,152,161,165},
	}

    -- set default image, emulate Android frame buffer
    -- editor option, development only!
    local s = LuaObject:imageDefault("scedit-default.png")
    print("default frame buffer emulate image = ", s)

	local ret = LuaObject:checkPixelsByPos(tbl_screen01)
	if ret then
		LuaObject:adbClick(558,528)
	end

	LuaObject:stateSleep(5)
	LuaObject:stateSet(stateOld + 1)
end

 