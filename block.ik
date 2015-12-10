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

while ([a]) {
	p(a);
	a = a - 1;
};

p("**************************************");

func = fn () {
	let i = 0;
	while ([i - 10]) {
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


func = fn () {
	let abcd = 10;
	eval("fn () {
		eval(\"[abcd = 100];\");
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

p(fib(10));
func();