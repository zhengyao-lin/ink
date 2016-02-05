#! /usr/bin/ink

import io

$array.find = fn (obj) {
	let i = 0
	base.each { | v |
		if (v == obj) {
			retn i
		}
		i++
	}
	-1
}

let get_prompt = fn () {
	"inki> "
}

let readln = fn (prompt) {
	readline.rl_read(prompt)
}

let print = fn (str) {
	stdout.puts(str.to_str());
}

let println = fn (str) {
	print(str.to_str() + "\n")
}

let object_traced_stack = new Array()
let object_to_str = fn (obj) {
	if (object_traced_stack.find(obj) >= 0) {
		retn "<traced>"
	}
	object_traced_stack.push(obj)

	let type = typename(obj)
	if (type == "numeric") {
		retn obj.to_str()
	} else if (type == "string") {
		retn "\"" + obj + "\""
	} else if (type == "array") {
		let ret = "[ "
		obj.each { | v |
			if (ret != "[ ") {
				ret = ret + ", ";
			}
			ret = ret + object_to_str(v)
		}
		ret = ret + " ]"
		retn ret
	} else if (type == "null" || type == "undefined") {
		retn type
	} else if (type == "function") {
		retn "fn () { ... }"
	} else {
		let ret = "{ "
		obj.each { | k, v |
			if (ret != "{ ") {
				ret = ret + ", ";
			}
			ret = ret + k + ": " + object_to_str(v)
		}
		ret = ret + " }"
		retn ret
	}
}

let init = fn () {
	scope = fn () { fn () {} } ()
	while (1) {
		let ret = scope::(eval(readln(get_prompt())))

		object_traced_stack = new Array()
		println("==> " + object_to_str(ret))
	}
}

init()
