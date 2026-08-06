project(
  'cShore',
  'c',
  version: '1.0.0',
  default_options: [
    'warning_level=3',
    'c_std=c99',
  ],
)

cc = meson.get_compiler('c')
system = host_machine.system()

cmake = import('cmake')

raylib_options = cmake.subproject_options()

raylib_options.add_cmake_defines({
  'BUILD_EXAMPLES': 'OFF',
})

if system == 'linux'
  raylib_options.add_cmake_defines({
    'GLFW_BUILD_WAYLAND': 'ON',
  })
endif

raylib_proj = cmake.subproject(
  'raylib',
  options: raylib_options,
)

raylib = raylib_proj.dependency('raylib')

sources = files(
  'src/main.c',
)

executable(
  'cShore',
  sources,
  dependencies: raylib,
  include_directories: include_directories('include'),
)
