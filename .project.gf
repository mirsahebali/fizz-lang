[gdb]
arguments=-nx -ex record
[executable]
path=./build/fizzlang-tests
arguments=
ask_directory=1
[commands]
Compile=shell make build
Run normal=file build/fizzlang-debug;run&
Run tests=file build/fizzlang-tests;run&
Set breakpoints=b test_operator_precedence_parsing;run ;b parse_expression_statement; b parse_expression;b parse_prefix_expression; b parse_infix_expression
[shortcuts]
Ctrl+N=n
Ctrl+Shift+N=reverse-next
Ctrl+Shift+S=reverse-step

