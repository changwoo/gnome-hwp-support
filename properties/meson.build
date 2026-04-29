nautilus_dep = dependency('libnautilus-extension-4')
glib_dep = dependency('glib-2.0')
gobject_dep = dependency('gobject-2.0')

extensiondir = nautilus_dep.get_variable(pkgconfig: 'extensiondir')

sources = [
  'hwp-properties-model-provider.c',
  'hwp-properties-model.c',
  'hwp-properties-module.c',
  'props-data.c',
]

shared_module('hwp-properties',
  sources: [
    'hwp-properties-model-provider.c',
    'hwp-properties-model.c',
    'hwp-properties-module.c',
    'props-data.c',
  ],
  dependencies: [nautilus_dep, libgsf_dep, glib_dep, gobject_dep],
  c_args: ['-DG_LOG_DOMAIN="hwp-properties"'],
  include_directories: config_inc,
  install: true,
  install_dir: extensiondir,
  gnu_symbol_visibility: 'hidden',
)

test_exe = executable('test-prop',
  sources: [
    'props-data.c',
    'test-prop.c',
  ],
  dependencies: [nautilus_dep, libgsf_dep, glib_dep, gobject_dep],
  include_directories: config_inc,
  install: false
)
