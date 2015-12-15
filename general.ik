for = inl (init, &cond, &iterate, block) {
	while (cond()) {
		block();
		iterate();
	}
}

max = fn (args...) {
	for (let i = 1 && let max = args[0],
		 i < args.size(), i = i + 1) {
		if (args[i] > max) { max = args[i]; }
	}
	max;
}

__Array = Array
ExArray = fn (args...) {
	this.prototype = new __Array(args);
	this.indexOf = fn (a) {
		for (let i = 0, i < base.size(), i = i + 1) {
			if (base.prototype[i] == a) {
				retn i;
			}
		}
		-1;
	}
	this.'==' = fn (a) {
		if (a.size() != base.size()) {
			retn 0;
		}
		base.each() { | elem |
			if (a.indexOf(elem) == -1) {
				retn 0;
			}
		}
		1;
	}

	this.'*' = fn (a) {
		let ret = new Array();
		base.each() { | elem1 |
			a.each() { | elem2 |
				let tmp = new Array(elem1, elem2);
				if (ret.indexOf(tmp) == -1) {
					ret.push(tmp);
				}
			}
		}
		ret;
	}
	this.p = fn (args...) {
		prefix = ""
		if (args.size()) {
			prefix = args[0];
		}

		p(prefix + "{");
		base.each() { | elem |
			if (elem.p) {
				elem.p(prefix + "  ");
			} {
				p(prefix + "  " + elem + ",");
			}
		}
		p(prefix + "}");
	}
}
Array = ExArray;

a = new Array(1, 2, 3);
b = new Array("a", "b", "c");
// p(a.indexOf(50));
// p(a == new Array(new Array(30, 20), new Array(10, 20), new Array(40, 5)));
(a * b).p();