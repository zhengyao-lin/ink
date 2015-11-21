func = (a) {
	a;
};

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
	base.base.b = val;
};

p(a.b);
p(a.b.d);
a.b.c();
p(a.b.d);
p(a.b);
a.b.c();

p(a.b.'+'(2));

a = 10;
func = () {
	b = 123456;
	() {
		return;
		a = 20;
		p(b);
	};	
};

p(a);
func()();
p(a);

