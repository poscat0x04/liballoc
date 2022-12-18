project('liballoc', 'c',
  version : '0.1',
  default_options : ['c_std=c17', 'warning_level=3'])

compiler = meson.get_compiler('c')

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = ['-DBUILDING_LIBALLOC']

# Always disable strict aliasing
if compiler.get_argument_syntax() == 'gcc'
  lib_args += ['-fno-strict-aliasing']
endif

incdir = include_directories('include')

lib = both_libraries('liballoc', 'src/liballoc.c', 'src/util.c',
  install : true,
  c_args : lib_args,
  gnu_symbol_visibility : 'hidden',
  include_directories : incdir,
)

shlib = lib.get_shared_lib()

test_exe = executable('liballoc_test', 'test/liballoc_test.c',
  link_with : shlib,
  include_directories: incdir,
)
test('liballoc', test_exe)

# Make this library usable as a Meson subproject.
liballoc_dep = declare_dependency(
  include_directories: incdir,
  link_with : shlib)

# Make this library usable from the system's
# package manager.
install_headers('include/liballoc.h', subdir : 'liballoc/include')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : 'liballoc',
  filebase : 'liballoc',
  description : 'Memory allocators',
  subdirs : 'liballoc',
  libraries : shlib,
  version : '0.1',
)
