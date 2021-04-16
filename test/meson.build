testHello = executable(
	'hello',
	'hello.cxx',
	dependencies: contortDep,
	gnu_symbol_visibility: 'inlineshidden',
	implicit_include_directories: false,
	build_by_default: false,
	install: false
)

run_target(
	'testHello',
	command: testHello
)
