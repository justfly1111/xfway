project('xfway', 'c',
	default_options: 'c_std=c11')
cc = meson.get_compiler('c')

wayland_server = dependency('wayland-server')
wayland_client = dependency('wayland-client')
wayland_protocols = dependency('wayland-protocols')
libweston = dependency ('libweston-4')
libweston_desktop= dependency ('libweston-desktop-4')
libinput = dependency('libinput')
pixman = dependency('pixman-1')
xkbcommon = dependency('xkbcommon')
egl = dependency('egl')
libevdev = dependency('libevdev')
gtk = dependency('gtk+-3.0')
m = cc.find_library('m', required : false)

inc = include_directories('protocols')

subdir ('protocols')

executable('xfway', 'server.h', 'shell.c', 'shell.h', 'main-wayland.c', 'xfway-protocol.h', dependencies : [wayland_server, wayland_client, wayland_protocols, libinput, libweston, pixman, xkbcommon, egl, libevdev, libweston_desktop, gtk],
include_directories : inc,
link_with : server_protos)

