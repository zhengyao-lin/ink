func = { | a, b, block, block2 |
	p(a + b);
	block(a + b);
	block2();
};

func(1, 2) { | c |
	p("from block, " + c);
} {
	p("from block2, ");
};

func = fn (bool) {
	if (bool) {
		p("true!!!");
		retn;
		p("what?");
	} {
		p("false!!!");
		retn;
	};
	p("what?");
};

func(0);

func = fn (bool) {
	if (bool) {
		if (bool + 1) {
			retn 1;
		} {
			retn 2;
		};
		p(1111);
	};
	retn 0;
};

p("result: " + func(-1));
p("hello, world");

func = fn (a, b) {
	a + b;
};

range = fn () {
	this.range = fn () {
		retn new Array(new Array(1, 2), new Array(5, 6));
	};
};

func.ar = new Array("wwww", "aaaa");

a = func[new range()];
p(a[1]);

p("**************************************");

a = 10;

while (a) {
	p(a);
	a = a - 1;
};

p("**************************************");

func = fn () {
	let i = 0;
	while (i - 10) {
		p(i);
		i = i + 1;
		if (!(i - 5)) {
			retn "I'm retning";
		};
	};
	retn "oo";
};

p(func());
p(!0);
p(!123);


func = fn () {
	let a = 10.1;

	p("a = " + a);

	if (a > 10) {
		p("a > 10");
	};

	if (a < 10) {
		p("a < 10");
	};

	if (a >= 10) {
		p("a >= 10");
	};

	if (a <= 10) {
		p("a <= 10");	
	};

	if (a == 10) {
		p("a == 10");
	};

	if (a != 10) {
		p("a != 10");
	};
};

func();

i = 10;

if (i == 0 && i = 1) {
	p(i);
};

p(i);

func = fn (a, b) {
	if (a == undefined) {
		a = "I'm default. ";
	};
	if (b == undefined) {
		b = "I'm default, too.";
	};

	a + b;
};

p(func(undefined, undefined));
str = "str";
p("str" == 'str');

str = " haha";

func = fn () {
	let str = 10;
	p("first: " + str);
	p("second");
	//this();
};

a = func.exp();
p("size: " + a.size());

(fn (){} << a[-3] << a[-2] << a[-1])();

p("=================Function Rebuild===================");

a.remove(-3);

func2 = a.rebuild();
func2();

p(0.2 + 0.4);

p(a[123]);

eval("eval(\"abc = \\\"hellowww\\\";\");");
p(abc);

lazy = fn (&exp) { exp }

func = fn () {
	let abcd = 10;
	eval("fn () {
		eval(\"lazy(abcd = 100);\");
	} ();")();
	p(abcd);
};

func2 = fn () {
	abcd;
	func();
	p(abcd);
};

func2();
p(abcd);

func = fn () {
	let a;
	top a = 10;
	p(a);
};

fib = fn (n) do
	/* if (dp[n]) do
		retn dp[n]
	end */
	if (n == 0 ||
		n == 1) do
		retn 1
	end
	
	retn this(n - 2) + this(n - 1)
end

// p(fib(10));
func();

yield_f = fn (block) {
	block;
};

consumer = fn () {
	let r = "200 OK"
	let ret = r

	ret.send = yield_f() { |n|
		if (!n) { retn }
		p("[CONSUMER] Consuming " + n + "...");
		ret;
	}
	retn ret;
}

producer = fn () {
	let c = consumer();
	let n = 0;
	while (n < 5) {
		n = n + 1;
		p("[PRODUCER] Producing " + n + "...");
		let r = c.send(n);
		p("[PRODUCER] Consumer return: " + r);
	}
}

producer();

yieldHost = fn (yieldFunction) {
	retn fn (processer) {
		yield_f = fn (result) { 
			processer(result)
		};
        yieldFunction(yield_f);
	};
}

func = fn (yield_f) {
	a = 0;
	while (a <= 10) {
		yield_f(a);
		a = a + 1;
	}
}

g = yieldHost(func);
g(fn (a) {
	p(a);
});


Object = fn () {};
///////////////////////////////////////////////////////////
fib = fn () {
	let generator = new Object();
	let '$yield' = fn (k, value) {
		generator.next = k;
		retn value;
	};

	generator.next = fn () {
		let i = 0;
		let j = 1;
		let '$while' = fn () {
			retn '$yield' (fn () {
				let t = i;
				i = j;
				j = j + t;
				retn '$while'();
			}, i);
		};
		retn '$while'();
	}
	retn generator;
}


p("**********************************************************");

let g = fib();
let i = 0;

while (i < 10) {
  p(g.next());
  i = i + 1;
}

p("**********************************************************");

lazy_exp = fn (&exp) {
	exp;
}

func = fn () {
	let this.a = "hello~~";
	retn lazy_exp(this.a = this.a + "~");
}

l = func();
i = 0;
while (i < 10) {
	p(l());
	i = i + 1;
}

func = fn (args...) {
	let i = 0;
	while (i < args.size()) {
		p(args[i]);
		i = i + 1;
	}
}

func(10, 20);

p(asdsa = 10);
p(asdsa);

i = 10;
p(i++);
p(++
i);

Object = fn (block) {if (block) { block(this) } }
a = new Object();

package = new Object() { | this_p |
	this_p.count = 1
	this_p.load = fn () {
		p("I'm package loader!! -- the " + base.count++ + "th time");
	}
}

import package, package, package

for = inl (init, &cond, &iterate, block) {
	block.'continue' = iterate;
	while (cond()) {
		block();
		iterate();
	}
}

for (let i = 0, i < 10, i++) {
	if (!(i % 2)) {
		continue;
	}
	p(i);
}
p("end!");


String = fn (str) {
	this = clone str;
	this.size = 10;
	this.getter("size") {
		retn this.base.length();
	}
	this.setter("size") {
		p("readonly!!");
	}
}

str = new String("hello, world");
p(str.size);
str.size = 10;
p(str.size);
p(str);


this.setter("value") { | val |
	this = val;
}

value = 10;
p(value);

p("###################################");

a = new Array(10, 20);

foreach = fn (a, &val, block) {
	a.each () { | v |
		val() = v
		block()
	}
}

var elem;
foreach (a, elem) {
	p(elem);
}

p("###################################");

a = 10;
a + 1;
a.b = 10;
a.each () { | name, value |
	p(name + " = value of type \"" + typename(value) + "\"");
}

func = fn (id) {
	p(id)
}

func() hhhh;

if (0) {
	p("yeha");
} else {
	p("wow");
}

/*missing = fn (str) {
	p("missing: " + str);
}*/

TypeA = fn () {
	this.missing = fn () {
		retn "haha, no"
	}
}

a = new TypeA();
a.b? = 10;
p(a.b?);

p((1 + ?)(2));
func = fn (a, b, c) {
	p(a + b + c);
}

func(1, ?, ?)(2)(3);