

-- skeleton.lua
-- Default ADB Viewer Lua script skeleton.


require "imageTable"


function main(n)
    local x = LuaObject:stateGet()
    print("state = ", x, ", old return = ", n)
    
    local default_object = {
       width  = 38,
       height = 32,
       itype  = 0,
       data = {{ 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }}
    };
    local int_object = { 1, 2, 3, 4, 5 }
    
    -- set default image
    local s = LuaObject:imageDefault("TestImageLite0.png")
    print("default frame buffer emulate image = ", s)
    
    -- get Android frame buffer, LuaImage format
    local img_screen0 = LuaObject:screenGet()
    print("screen: width = ", img_screen0.width, ", height = ", img_screen0.height)
    print("screen type = ", img_screen0.itype)
    LuaObject:imageTableShow(img_screen0)
    
    -- get region from Android frame buffer, LuaImage format
    local img_screen1 = LuaObject:screenGetCord(50, 20, 200, 200)
    print("screen region: width = ", img_screen1.width, ", height = ", img_screen1.height)
    print("screen region type = ", img_screen1.itype)
    LuaObject:imageTableShow(img_screen1)
    
    -- iterate LuaImage
       for k,v in ipairs(img_screen1.data) do
            print(k, " = ", v[1], ":", v[2], ":", v[3])
       end
    
    LuaObject:imageTableShow(default_object)
    LuaObject:imageTableShow(int_object)
    LuaObject:imageTableShow(default_img)
    
    local pos = LuaObject:imageGetPosition(100, 200)
    --* checkPixelByCord(x, y, R, G, B)
    local b1  = LuaObject:checkPixelByCord(970, 400, 77, 182, 172)
    local b2 = false
    if pos ~= nil then
        --* checkPixelByPos(position, R, G, B)
        b2  = LuaObject:checkPixelByPos(pos, 204, 204, 204)
    end
    -- compare image
    -- default screen test image is name "scedit-default.png"
    local d = LuaObject:imageCompare("TestImageLite0.png")
    --* imageCompareRegion(file name, x, y, w, h)
    local r = LuaObject:imageCompareRegion("TestImageLite0.png", 0, 0, 100, 200)
    -- show/edit image
    LuaObject:imageShow("TestImageLite0.png")
    LuaObject:imageShow("TestImageLite1.png")
    LuaObject:imageShow("TestImageLite2.png")
    LuaObject:imageShow("TestImageLite3.png")
    
    -- logical code body
    print("main = ", n)
    n = n + 1
    local z = 0
    if x ~= nil then
        z = x + n
    end
    local s = "test string"
    local t = {}
    t[0] = 10
    t[1] = 100
    t[2] = 1000
    -- see sense help and annotation
    
    x = x + 1
    LuaObject:stateSet(x)
    return n + 5
end

return 0
 