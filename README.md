# snow-lang
The snow programming language's home

# cpptooling
The compiler is writting in C++ until such a time that the compiler can written entirely in snow.

## Requirements
### Windows
Visual studio is the primary supported editor for now, CLion is coming soon.
- VS2022 or higher
- Python 3
- cmake

### Linux
You can use this with cmake raw, but CLion configuration files are also available.
- cmake
- CLion (optional)


## Building
### Windows
From the cpptooling folder, run `py cmake/genproj.py` to generate the project which will then be found at `build/win32/snowc.sln`.

### Linux
For CLion, open the cpptooling directory as a cmake project.

# docs
Snow documentation can be found under the docs folder.

# Contributing
At the moment, this project is closed for external contributions.
