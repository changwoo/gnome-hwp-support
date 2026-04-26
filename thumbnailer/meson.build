thumbnailer_deps = [
  dependency('glib-2.0'),
  dependency('gdk-pixbuf-2.0'),
  dependency('libgsf-1'),
]

hwp_thumbnailer = executable('hwp-thumbnailer',
  'hwp-thumbnailer.c',
  include_directories: config_inc,
  dependencies: thumbnailer_deps,
  install: true,
)

datadir = get_option('datadir')
thumbnailer_dir = join_paths(datadir, 'thumbnailers')

install_data('hwp-thumbnailer.thumbnailer',
  install_dir: thumbnailer_dir
)

