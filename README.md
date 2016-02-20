# seal2d
A 2D game engine based on OpenGL, developed with Lua and C.

This project is under heavy development currently, I'm trying to do almost everything in Lua, big changes would be made :D.

# build & run

You need to set the custom working directory in xcode.
    
    1. Open the example in "example/yuusha/proj.mac" or "proj.ios" by XCode. 
    2. Goto Product->Scheme->Edit Scheme.
    3. Click the left 'Run' tab, and find 'options'.
    4. Select the 'Use custom working directory:', and select the example's root directory, should be
    $(WHERE_YOU_PUT_THIS_PROJECT)/seal2d/example/yuusha
    5. Hit cmd+R, or click run to run the project. :)
  
If you got anyquestion, please don't bother to add an issue. thank you. :)

Due to there may be big changes in this project, so I may only provide iOS and Mac platform support right now.

# the Luajit branch
Please checkout the lua-jit branch to use that if you preffered. The lua branch is FASTER than the luajit branch, looking forward someone may fix that :).
