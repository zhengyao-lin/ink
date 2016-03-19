import blueprint
import blueprint.math

$array.sum = fn () {
	let ret = null
	base.each { | v |
		if (!ret) {
			ret = v
		} {
			ret += v
		}
	}
	ret
}

$array.trim_end = fn (t_v) {
	t_v ||= undefined
	for (let i = base.size() - 1,
		 i >= 0 && base[i] == t_v, i--)
	base.slice(0, i - 1)
}

let PolyNom = fn (coefs...) { // A() = sigma(i = 0 to n - 1)(ai * x ^ i)
	this = clone coefs

	this.`+` = fn (polyn) {
		let i = 0
		let smaller = null
		new PolyNom() with ((if (base.size() > polyn.size()) {
			smaller = polyn, base
		 } {
		 	smaller = base, polyn
		 }).each { | v |
			let ret = if (i < smaller.size()) {
				smaller[i] + v
			} {
				v
			}
			i++
			ret
		})
	}

	/* not the most optimized implementation */
	this.`*` = fn (polyn) {
		let size = base.size() + polyn.size()
		let i = 0
		let j = 0

		base.each { | v1 |
			let ret = new Array(size, 0)
			j = 0
			polyn.each { | v2 |
				ret[i + j] = v1 * v2
				j++
			}
			i++
			new PolyNom() with (ret.trim_end(0))
		}.sum()
	}

	this.p = fn () {
		let ret = ""
		let i = 0
		base.each { | v |
			if (ret != "") {
				ret += " + "
			}
			ret += (if (i) { v.to_str() + "x^" + i } { v.to_str() })
			i++
		}
		p(ret)
	}

	this@call = fn (x) {
		let ret = 0
		let i = 0
		base.each { | v |
			ret += v * math.pow(x, i)
			i++
		}
		ret
	}
}

poly1 = new PolyNom(1, 2, 3)
poly2 = new PolyNom(1, 2, 4, 5, 6, 7, 8, 9, 10)
poly3 = new PolyNom(9, -10, 7, 6)
poly4 = new PolyNom(-5, 4, 0, -2)

poly1.p()
poly2.p()
poly3.p()
poly4.p()

(poly1 + poly2).p()
(let poly5 = poly3 * poly4).p()
p(poly1(10))
p(poly2(10))
p(poly5(12))
