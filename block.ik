func = { | a, b, block, block2 |
	p(a + b);
	block(a + b);
	block2();
};

func(1, 2) { | c |
	p("from block, " + c);
}{
	p("from block2, ");
};

func = (bool) {
	if (bool) {
		p("true!!!");
		return;
		p("what?");
	} {
		p("false!!!");
		return;
	};
	p("what?");
};

func(0);

func = (bool) {
	if (bool) {
		if (bool + 1) {
			return 1;
		} {
			return 2;
		};
		p(1111);
	};
	return 0;
};

p("result: " + func(-1));
p("hello, world");

func = (a, b) {
	a + b;
};

range = () {
	this.range = () {
		return new Array(new Array(1, 2), new Array(5, 6));
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

func = () {
	let i = 0;
	while ([i - 10]) {
		p(i);
		i = i + 1;
		if (!(i - 5)) {
			return "I'm returning";
		};
	};
	return "oo";
};

p(func());
p(!0);
p(!123);


func = () {
	let a = 11;

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

func = (a, b) {
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

func = () {
	let a = 10;
	p("first" + a);
	p("second");
	//this();
};

a = func.exp();
p("size: " + a.size());
a[0]();
a[1]();

p("=================Function Rebuild===================");

func2 = a.rebuild();
func2();