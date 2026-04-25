---

kanban-plugin: board

---

## Backlog

- [ ] Better Attributes
- [ ] [[UTF8 Support]]
	- Convert rune and string to be utf8 supported
	- Support `\uxxx` and `\Uxxxxxxxx` escape sequences as unicode characters taking hex values
	- Support `rune.Invalid` being set to the invalid UTF8 character
	- `rune.isValid()` should compare against the invalid UTF8 character
	- `rune.size()` should return if this is a 1, 2, or 4 byte character
	- `string.dataSize()` should return the number of bytes the string is
- [ ] Built-in types have proper documentation with operators, statics, members, ...
- [ ] Integer base change
	- Static function
	- Converts from base to another if safe to do so (2, 8, 10, 16)
	- `fn baseChange(i8 from, i8 to, T in): K?`
		- Where T is integral, K is integral
		- Returns undefined if unable to do conversion or converted value will not fit in integral of K
- [ ] Documentation Comments
	- `///` will document next object
	- `//!` will document the current object
		- Not sure about this. Might remove this or make this a block comment version of above. Maybe file-module docs use a moniker like `--file-docs--` or something
	- Generates markdown documentation
- [ ] BLOCKED | Compiler flags
	- Decide on these, dont have too many
	- Isnt command line based, is actually a part of the package step, so putting this off


## Current Feature Work

- [ ] Source Control Markers
	- `<<<<<<` (6)
	- `======` (6)
	- `>>>>>>` (6)
	- These markers should be considered errors (or high class warnings)
- [ ] V0.0.1: Parser Stage 1:
	- Open files as imports come through
	- Compilations go into a task list, task list is threaded, each file is a task
	- Parse the files except for the function implementation, and param assignment.
	- When all files are parsed, assemble the type data.
	- Result is a partial AST built of mostly types.
	- Type Parse Emitter outputs all type data collected
	- Alias, comments, functions, Built in types
- [ ] V0.0.1: Parser Stage 2:
	- Built in type usages, variables, functions, casting, loops, defer, panic, std.out, std.err
	- Finish ast creation
	- Start ast passes
	- AST Emitter outputs final AST
- [ ] Restricted Interpreter:
	- Can use anything in std, and additional packages provided, nothing else
	- This point should be able to now build projects for testing and receive ouput
- [ ] Finish V0.0.1
	- Can actually execute some code now. Restricted to the interpreter, handles only default `package` style


## Active

- [ ] Lexer produces all token types, and a Token Emitter
	- Supports all tokens that the language uses
	- Not sure what Token Emitter is, so just leaving it out for now


## Complete

- [ ] Find out how to handle folders and module traversal, I have submodules figured out but not actual folders
- [ ] Command line arguments
	- Logging level to be printed, debug flags like print the ast to a file, tokens, ...
- [ ] Custom Logger, supports verbosity
- [ ] Change to clang style file loading and locs
	- ~Change diags~
	- ~Redo lex and token~ // moved to its own ticket




%% kanban:settings
```
{"kanban-plugin":"board","list-collapse":[false,false,false,false]}
```
%%