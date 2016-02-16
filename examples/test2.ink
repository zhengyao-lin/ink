import blueprint

namespace = {
	missing: fn (name) {
		top[name] = fn () { fn () {} } ()
	}
}

using = {
	missing: fn (name) {
		top[name]::(let each { | key, value |
			if (key != "this" &&
				key != "base" &&
				key != "let" &&
				key != "each" &&
				key != "prototype") {
				top[key] = value
			}
		})
		null
	}
}

namespace a::(
	let b = 10
)

using a

p(b);