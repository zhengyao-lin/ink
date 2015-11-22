func = (a) { a; };

func2 = () {};

func2.p = (a) {
	func(a);	
};

(f) {
	f.p = () { 0; };
} (func2);

p(func2.p(120));

g1 = 10;

func = () {
	g1;	
};

func2 = (f) {
	g1 = 20;
	f();
};

p(func2(func));

i = 10;
i.p = 20;

a = i;

p(a.p);

a = 1000;
a.b = 20;
a.b.d = 10;
a.b.c = () {
	base.d = 20;
	val = 100;
	val.c = this;
	base = val;
};

p(a.b);
p(a.b.d);
a.b.c();
p(a.b.d);
p(a.b);
a.b.c();

a = 10;
func = () {
	b = 123456;
	() {
		a = 20;
		p(b);
	};	
};

p(a);
func()();
p(a);

func =
() {
	b = 70;
	() {
		//b = 20;
		(a) {
			p(a + b);
		};
	};
} () ();

func(10010);

a = 10;
b = 20;

p((a + b) * 2 + 20);

func = () {
	(block) { p(block() * 2); };
};

func () {
	12;
};

func1 = () {
	return 100;
};

func = (bool) {
	func1();
	if (bool) {
		if (bool + 1) {
			return 1;
		}.else {
			return 2;
		};
		p(1111);
	};
	return 0;
};

inline = (block) { block; };
pop = inline {
	return 100;
};

p(func(-1));