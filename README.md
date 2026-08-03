# cSand

A really basic sand simulation in C & Raylib.

## Controls

- `LEFT MOUSE BUTTON`  : Used to draw the current particle by dragging the mouse with the button pressed.
- `MOUSE SCROLL WHEEL` : Used to change currently selected particle type.
- `CTRL + L` : Delete all particles at once.    

## Build

- You'd need a `c compiler`, `meson`, `ninja` and `cmake` (To build Raylib).
- `Raylib` is provided as a git submodule.

```bash
git clone --recursive https://github.com/tmpstpdwn/cSand
cd cSand
meson setup build --buildtype=release
meson compile -C build
```

## Run

```bash
./build/cSand
```

## LICENSE

`EMPTY`

