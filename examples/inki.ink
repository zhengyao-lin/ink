#! /usr/bin/ink

import io

let show_director = fn () {
	stdout.puts("inki> ")
}

let readln = fn () {
	stdin.gets()
}

let print = fn (str) {
	stdout.puts(str.to_str());
}

let println = fn (str) {
	print(str.to_str() + "\n")
}

let object_to_str = fn (obj) {
	let type = typename(obj)
	if (type == "numeric") {
		retn obj.to_str()
	} else if (type == "string") {
		retn "\"" + obj + "\""
	} else if (type == "array") {
		let ret = "["
		obj.each { | v |
			if (ret != "[") {
				ret = ret + ", ";
			}
			ret = ret + object_to_str(v)
		}
		ret = ret + "]"
		retn ret
	} else if (type == "null" || type == "undefined") {
		retn type
	}
}

let init = fn () {
	while (1) {
		show_director()
		let ret = eval(readln())

		println("==> " + object_to_str(ret));
	}
}

init()
