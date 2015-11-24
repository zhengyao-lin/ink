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

p("result: " + func(null));
p("hello, world");

str = "hello, world";
p(str);

if (nullobj) {
	p("wow, it turns to object!");	
}.else {
	p("it's still false!");
};

a = 10;
b = 20;
c = a + b;
d = null;
e = undefined;

p(a + b + c);

a.b = "I'm a.b";
hash = "b";

p("hello"[0]);
p(a[hash]);

Object.setLocal = (name, value) {
	base[name] = value;
};

a = Object;
a.setLocal("val1", "halo!");
p(a.val1);

// Closure
func = () {
	// a = 110;
	() { p(a); };
};

func2 = () {
	a = 220;
	func()();
};

func2();

a[0] = 20;
p(a[0]);

TypeA = () {
	this.value = 1001;
};

TypeB = () {
	this = new TypeA();

	this.value = 10;
};

a = TypeB.'new'();

p(a.value);

Object = () { };

b = new Object();
b.p = 10;
a = clone b;
b.p = 20;
p(a.p);

Car = () {
	this.max_speed = 100;
	this.drive = () {
		p("hu---");
	};
};

Ferarri = () {
	this = new Car();
	max_speed : 300;
	drive : () {
		p("su---");
	};
};

car = new Ferarri();
p(car.max_speed);
car.drive();

p("\n---------lazy expression---------\n");

a = 5;
lazy = [a = 1 + 2 + 3];
lazy << [20];
p("5? : " + a);
p("20? : " + lazy());
p("6? : " + a);

Builder = () {
	TypeC = () { };
	TypeC << () {
		a : "hello";
		b : 20;
	};
	TypeC;
};

a = new () {
	a : "hello";
	b : 20;
} ();
p("hello? : " + a.a);

func = (b) {
	if (b) {
		return "true";	
	};
	return "false";
};

p("true? : " + func("hi"));

func = () {
	a : 100;
	lazy_exp = [this];
	p("100? : " + lazy_exp().a);
};

func();

//for (i = 0, i < 10)
a = 10;
b = [a];
//p(a[a]);
a = 20;

[p("20? : " + b())]();