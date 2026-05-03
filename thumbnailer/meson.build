thumbnailer_deps = [
  dependency('glib-2.0'),
  dependency('gdk-pixbuf-2.0'),
  dependency('libgsf-1'),
]

libexecdir = join_paths(get_option('prefix'), get_option('libexecdir'))
thumbnailer_dir = join_paths(get_option('datadir'), 'thumbnailers')

hwp_thumbnailer = executable('hwp-thumbnailer',
  'hwp-thumbnailer.c',
  include_directories: config_inc,
  dependencies: thumbnailer_deps,
  install: true,
  install_dir: get_option('libexecdir'),
)

conf_data = configuration_data()
conf_data.set('LIBEXECDIR', libexecdir)

configure_file(
  input: 'hwp-thumbnailer.thumbnailer.in',
  output: 'hwp-thumbnailer.thumbnailer',
  configuration: conf_data,
  install: true,
  install_dir: thumbnailer_dir
)