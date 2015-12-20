import "tests/general.ik",
	   "tests/container.ik"

a = new Array(1, 2, 3)
b = new Array("a", "b", "c");


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