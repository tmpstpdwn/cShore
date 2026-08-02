# cSand

A really basic sand simulation in C & Raylib.

## Controls

- `LEFT MOUSE BUTTON` : Used to draw by dragging the mouse with the button pressed.
- `RIGHT MOUSE BUTTON` : Let the simulation run with a simple button click.

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

