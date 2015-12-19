p(10 + 20);

a = 10;
func = fn () {
	a = 20;
	p(a);	
};
func();

func = fn (a) { a; };

func2 = fn () {};

func2.p = fn (a) {
	func(a);	
};

fn (f) {
	f.p = fn (a) { 0; };
} (func2);

p(func2.p(120));

g1 = 10;

func = fn () {
	g1;
	p("hi");
	"III";
};

func2 = fn (f) {
	g1 = 20;
	p(g1);
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
a.b.c = fn () {
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
func = fn () {
	b = 123456;
	fn () {
		a = 20;
		p(b);
	};	
};

p(a);
func()();
p(a);

func =
fn () {
	b = 70;
	fn () {
		//b = 20;
		fn (a) {
			p(a + b);
		};
	};
} () ();

func(10010);

a = 10;
b = 20;

p((a + b) * 2 + 20);

func = fn (block) {
	p(block() * 2);
};

func () {
	12;
};

func1 = fn () {
	retn 100;
};

func = fn (bool) {
	func1();
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

inline = fn (block) { block; };
pop = inline () {
	retn 100;
};

p("result: " + func(null));
p("hello, world");

str = "hello, world";
p(str);

if (nullobj) {
	p("wow, it turns to object!");	
} {
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

Object = [
	fn (name, value) {
		base[name] = value;
	}
];

a = Object;
a[0]("val1", "halo!");
p("halo!? : " + a.val1);

// Closure
func = fn () {
	// a = 110;
	fn () { p(a); };
};

func2 = fn () {
	a = 220;
	func()();
};

func2();

TypeA = fn () {
	this.value = 1001;
};

TypeB = fn () {
	this = new TypeA();

	this.value = 10;
};

a = TypeB.'new'();

p(a.value);

Object = fn () { };

b = new Object();
b.p = 10;
a = clone b;
b.p = 20;
p(a.p);

Car = fn () {
	this.max_speed = 100;
	this.drive = fn () {
		p("hu---");
	};
};

Ferarri = fn () {
	this.prototype = new Car();
	max_speed : 300;
	drive : fn () {
		p("su---");
	};
};

car = new Ferarri();
p(car.max_speed);
car.drive();

p("\n---------Lazy Expression---------\n");

lazy = fn (&exp) { exp }

a = 5;
lazi = lazy(a = 1 + 2 + 3);
lazi << lazy(20);
p("5? : " + a);
p("20? : " + lazi());
p("6? : " + a);

Builder = fn () {
	TypeC = fn () { };
	TypeC << fn () {
		a : "hello";
		b : 20;
	};
	TypeC;
};

a = new fn () {
	a : "hello";
	b : 20;
} ();
p("hello? : " + a.a);

func = fn (b) {
	if (b) {
		retn "true";	
	};
	retn "false";
};

p("true? : " + func("hi"));

func = fn () {
	a : 100;
	lazy_exp = lazy(this);
	p("100? : " + lazy_exp().a);
};

func();

a = 10;
b = lazy(a);
//p(a[a]);
a = 20;

lazy(p("20? : " + b()))();

p("\n---------Array & Bonding---------\n");

a = 10;

d = b -> a;
c.efg -> b;

a = 20;
p("20? : " + c.efg);
p("10? : " + d);

a = new Array(1, 2);
a.push("hello");
p("hello? : " + b[2]);
p("3? : " + a.size());

p("2? : " + c.efg[1]);

arr = new Array(1, 2, 3);
b -> arr;
p("1? : " + b[0]);

foo = new Array(1, 2, 3);
bar = new Array("wow");
bar[0] -> foo;
p(bar[0][0]);

a = 0;
index = 10;
index -> a;
!! bar[index];
p(bar[0]);
!! index;
p(index);

// pointer-like
val = "before";
ptr = lazy(val);
func = fn (ptr) {
	ptr() = "after";
};

func(ptr);
p(val);

a = 10;
!! b;
b -> a;
func = fn () {
	b = 0;	
};

func();
p(a);

a = "hello";
foo = fn () {
	var let a.b = 10;
	bar = fn () {
		//let a = 0;
		//p("0? : " + a);
	};

	p("10? : " + a.b);
	bar;
};

foo()();
p("hello? : " + a);

a = 0;

func = fn () {
	let a = 10;
	lazy(a);
};

p("10? : " + func()());

func = fn () {
	10;	
};

func2 = clone func;
func2 << fn () {
	20;	
};

p("10? : " + func());
p("20? : " + func2());

TypeD = fn () {
	this.a = 10;	
};

TypeE = fn () {
	this = new TypeD();
	this.a = "hello?";
};
b = new TypeE();
p(b.a);

func = fn () {
	let b = 10;
	let a -> b;
	!! a;
	retn lazy(top aaa);
};
aaa = "wow!";
p(func()());

func = fn () {
	
};

TypeZ = fn () {
	str : "From Type Z";
};

TypeA = fn () {
	this.prototype = new TypeZ();
	a : "I'm A ";
};

TypeB = fn () {
	this.prototype = new TypeA();
	this.b = "hi, ";
};

obj = new TypeB();
p(obj.b + obj.a + obj.str);

p(ufadss);

p("out: " + 3.3);