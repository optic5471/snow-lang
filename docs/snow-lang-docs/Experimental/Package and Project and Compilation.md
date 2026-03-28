Every package must define a package statement.
The package statement defines how to build this package with dependencies and such.

Instead of providing every file in the package to the command line, you only need to define the root file of the package, whatever it's name and it will compile beyond that

`snowc build root.snw`

This lets multiple packages exist in the same area of the file system while only building the one you want.
What about build all? Maybe you can provide a different file that does a package import of the others?
```
package none all {
	const otherPackage = packageImport("../something/main.snw");
	all.nonDeps(otherPackage);
}
```

# Package (statement)
- Types
	- None: Builds some things, but essentially is a meta target
	- Inter: Restricted Interpreter executable code
	- Exe: Compiled executable
	- Lib: Compiled static library
	- DynLib: Compiled dynamic library

# Package (class)
 - `nonDeps: [PackageImport]`
	 - Adds a package to built, not as a dependency
 - `deps: [PackageImport]`
	 - Adds a package as a dependency to this one. Imports can call import starting with the package name.
	 - All dependencies will be compiled before this package is
 - `cargs: [CArg]`
	 - The compiler arguments that are passed to the compiler
 - Exe Only
	 - `startFn: enum { (fn():i32), (fn()) } = main;`
 - Exe, Lib, DynLib
	 - `enum AssemblyType { Arm, x86_64 }`
	 - `assemblyType: AssemblyType = Snowc.assemblyType;`: what assembly language to use if compiling to assembly
	 - `enum CompilationType { Jit, Asm, AsmObj }`: AsmObj produces object files similar to a c compiler, Asm is the full executable
		 - Jit is not allowed in Lib
	 - `compilationType: CompilationType = CompilationType.Asm;`: what type of compilation output
 - `preTasks: [string]`: command line tasks to run before compiling this package
 - `postTasks: [string]`: command line tasks to run after compiling this package

# Package Utilities
- `packImport(path: string): PackageImport`: Essentially a reference to another package that will be added to the list of things to do
- `env`:
	- The env object exists as a way to capture the environment including variables and arguments passed to the program. When compiling, the package object can use this to capture what was passed to it for compilation
- 