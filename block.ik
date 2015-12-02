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

a = 11;
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

i = 10;

if (i == 0 && i = 1) {
	p(i);
};

p(i);