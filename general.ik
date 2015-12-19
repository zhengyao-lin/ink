for = inl (init, &cond, &iterate, block) {
	block.'continue' = iterate;
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
	this = new __Array(args);
	this.indexOf = fn (a) {
		for (let i = 0, i < base.size(), i = i + 1) {
			if (base[i] == a) {
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

a = new Array(1, 2, 3)
b = new Array("a", "b", "c");
// p(a.indexOf(50));
// p(a == new Array(new Array(30, 20), new Array(10, 20), new Array(40, 5)));
//(a * b).p();


ret = (new Array(1, 2, 3)).each() { | elem |
	elem * 2 + 1;
}
ret.p = a.p;
ret.p();

func = fn (f, s) {
	p("" + f + "--->" + s);
}

c = a * b;
c.p();


c = null;
c.'[]' = (new Array()).prototype.'[]';
p(c[1]);

func = fn () { }
a = new Array();
a.'<<' = func.'<<';
a << 0;

Stack = fn () {
	this = new Array()

	this.pop = fn () {
		if (base.size() < 1) {
			retn null;
		}
		base.remove(base.size() - 1);
	}
}

st = new Stack();
input = "2 3 + 3 + 7 / 9 * 9 1 ++";

for (let i = 0, i < input.length(), i++) {
	let c = input[i];
	let tmp_num = "";

	while (c >= "0" && c <= "9") {
		tmp_num = tmp_num + c;
		i++;
		c = input[i];
	}
	if (tmp_num != "") {
		st.push(numval(tmp_num));
	}

	if (c == "+") {
		v1 = st.pop();
		v2 = st.pop();
		if (!v1 || !v2) {
			p("Error: no enough stack for operator '+'");
		} {
			st.push(v1 + v2);
		}
	} {
		if (c == "-") {
			v1 = st.pop();
			v2 = st.pop();
			if (!v1 || !v2) {
				p("Error: no enough stack for operator '-'");
			} {
				st.push(v1 - v2);
			}
		} {
			if (c == "*") {
				v1 = st.pop();
				v2 = st.pop();
				if (!v1 || !v2) {
					p("Error: no enough stack for operator '*'");
				} {
					st.push(v1 * v2);
				}
			} {
				if (c == "/") {
					v1 = st.pop();
					v2 = st.pop();
					if (!v1 || !v2) {
						p("Error: no enough stack for operator '/'");
					} {
						st.push(v1 / v2);
					}
				}
			}
		}
	}
}

st.p();