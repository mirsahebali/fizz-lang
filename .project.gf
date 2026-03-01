[gdb]
arguments=-nx -ex record
[commands]
Compile=shell make build
Run normal=file build/fizzlang-debug;run&
Run tests=file build/fizzlang-tests;run&
Run rppl=file build/fizzlang-rppl;run&
Set tests breakpoints=record target-all; b test_if_expression_parsing; run;  b parse_expression_statement; b parse_expression;b parse_prefix_expression; b parse_infix_expression;b parse_if_expression
Set rppl breakpoints=record target-all; run; b parse_let_statement; b parse_expression_statement; b parse_expression; b parse_identifier
[shortcuts]
Ctrl+N=n
Ctrl+Shift+N=reverse-next
Ctrl+Shift+S=reverse-step
