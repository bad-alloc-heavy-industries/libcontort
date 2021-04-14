# SPDX-License-Identifier: BSD-3-Clause

contortSrcs = [
	'decoration.cxx',
	'screen.cxx', 'mainLoop.cxx'
]

substrate = dependency(
	'substrate',
	required: true,
	version: '>=0.0.1',
	fallback: ['substrate', 'substrate_dep'],
	default_options: [
		'build_tests=false'
	]
)

contortLibrary = library(
	'contort',
	contortSrcs,
	include_directories: contortInclude,
	gnu_symbol_visibility: 'inlineshidden',
	implicit_include_directories: false,
	pic: true,
	install: true
)

contortDep = declare_dependency(
	include_directories: contortInclude,
	link_with: contortLibrary,
	version: meson.project_version()
)
