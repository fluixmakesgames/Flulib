# Flulib
<img src="logo.png" alt="drawing" width="200"/>

Raylib compatible(ish) library for window's games!

## Reccomended NPPExec Script:
```
SET CC=Disk:\raylib\w64devkit\bin\gcc
SET CFLAGS=-Os -s -std=c99 -Wall -ffunction-sections -fdata-sections
SET LDFLAGS=-lgdi32 -luser32 -lkernel32 -Wl,--gc-sections -Wl,--strip-all -mwindows

cd $(CURRENT_DIRECTORY)

cmd /c IF EXIST $(NAME_PART).exe del /F $(NAME_PART).exe

$(CC) -o $(NAME_PART).exe $(FILE_NAME) $(CFLAGS) $(LDFLAGS)

cmd /c IF EXIST $(NAME_PART).exe $(NAME_PART).exe
```

# Implemented Features
## Functions:
`InitWindow();`
`DrawRect();`
`DrawText();`
`DrawFPS();` (W.I.P)
`BeginDrawing();`
`EndDrawing();`
`ClearBackground();`
`CheckCollisionRecs();`
`IsKeyDown();`
`IsKeyPressed();`

## Types:
`Color` (Unused)
`Vector2`
`Vector3`
`Rectangle`

## Colours:
`RAYWHITE`
`LIGHTGRAY`
`GRAY`
`DARKGRAY`
`YELLOW`
`GOLD`
`ORANGE`
`PINK`
`RED`
`MAROON`
`GREEN`
`LIME`
`DARKGREEN`
`BLUE`
`DARKBLUE`
`PURPLE`
`VIOLET`
`DARKPURPLE`
`BEIGE`
`BROWN`
`DARKBROWN`
`BLACK`
`MAGENTA`
