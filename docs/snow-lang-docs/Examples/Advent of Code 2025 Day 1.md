```
/*
Given a safe starting at 50 which contains numbers 0-99, and a strip of instructions like:
L68\nL30\nR48\nL5\nR60\nL55\nL1\nL99\nR14\L82
Count the number of times that the safe points at 0 while performing the instructions.
In the above, it will be 3.
*/

import cli;

enum LR {
	L,
	R
};

fn main() {	
	// parse commands
	let rawCommands = cli.args[1].split('\n');
	mut commands: vec<(LR,i8)>(len: rawCommands.len()); // setting specific value for function call
	for (v in rawCommands) {
		if (v.empty()) { continue; }
		commands.push(v[0] == 'L' ? LR.L : LR.R,
			i8.parse(v[1:]).unwrap());                 // warning: unsafe unwrap
	}
	
	let safeVal = 50;
	mut count = 0;
	for (c in commands) {
		if (c.0 == LR.L) {
			safeVal -= c.1;
		}
		else {
			safeVal += c.1;
		}
		
		if (safeVal > 99) {
			safeVal -= 99;
		}
		else if (safeVal < 0) {
			safeVal += 99;
		}
		else if (safeVal == 0) {
			count++;
		}
	}
	
	cli.println(`${count}`); // `` will be a format string, same as "", but ${} will variable capture. \$ to escape it. Works with things that implement Printable interface
}
```