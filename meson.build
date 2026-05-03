project('gnome-hwp-support', 'c',
  version: '0.3.0',
)

localedir = join_paths(get_option('prefix'), get_option('datadir'), 'locale')

i18n = import('i18n')
conf_data = configuration_data()
conf_data.set_quoted('GETTEXT_PACKAGE', meson.project_name())
conf_data.set_quoted('LOCALEDIR', localedir)
configure_file(output: 'config.h', configuration: conf_data)
config_inc = include_directories('.')

libgsf_dep = dependency('libgsf-1')

subdir('po')
subdir('properties')
subdir('thumbnailer')
