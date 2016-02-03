#! /usr/bin/ink

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
		let $while = fn () {
			retn $yield (fn () {
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
	this_p.load = fn (pkg, apply_to) {
		p("I'm package loader!! -- the " + base.count++ + "th time");
	}
}

import package, package, package

for = inl (init, &cond, &iterate, block) {
	block.@continue = inl (arg) { iterate() }
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
		drop this.length();
	}
	this.setter("size") { | val |
		p("readonly!!");
	}
}

str = new String("hello, world");
p(str.size);
str.size = 10;
p(str.size);
p(str);

this.setter("value") { | val |
	top __value = val;
}
this.getter("value") {
	top __value
}

value = 10;
p(value);

p("###################################");

a = new Array(10, 20);

foreach = fn (a, &val, block) {
	a.each { | v |
		val() = v
		block()
	}
}

let elem = null;
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


if (0) {
	p("yeha");
} else {
	p("wow");
}

/*missing = fn (str) {
	p("missing: " + str);
}*/

TypeA = fn () {
	this.a = null
	this.missing = fn (name) {
		p("haha, no")
		retn base.a
	}
}

a = new TypeA();
a.b? = 10;
p(a.a);

p((1 + _)(2));
func = fn (arg...) {
	let ret = 0
	for (let i = 0, i < arg.size(), i++) {
		ret = ret + arg[i]
	}
	p(ret)
}

p("######################");

PAF = func() with [1, _, 2, _, 8, _, 0];
PAF2 = PAF(2) with [2] with [2, _, 5, 3]
PAF3 = PAF2(_, 2, 3, 4)(2);

p(1 + 2 + 8 + 0 + 2 + 2 + 2 + 5 + 3 + 2 + 2 + 3 + 4)

a = 0;
p(a++ = 10)

import io

fp = {}

p("+++++++++++++++++++++++++++++++++++");

if (file_exist("test_file.ik")) {
	p("freak!!! abnormal ending");
	fp = new File("test_file.ik", "r");
} else {
	p("that's right!");
	fp = new File("test_file.ik", "w+");
	fp.puts("p(\"I'm new file\")");
	fp.flush();
}

import "test_file.ik"
p(typename(fp));
fp.close();
fp = new File("test_file.ik", "rw");
p("from fp: " + fp.gets());
fp.close();
file_remove("test_file.ik");
p(typename(fp));

stdout.puts("from file interface\n");

p(typename(fn(){}));

p({
	name: "hello",
	id: 110
}.name);

a = fn () {};
p.a

//debug(new String("hi"));

a = fn () {
	where();
}

a();

debug(p);

glob = 10;

a = fn () {
	glob = inl () {
		retn inl () { retn }();
	} (), glob = 20;
}

a();
p(glob);

p("################ drop signal test ################");

inline = inl () {
	p("returning 10");
	drop 10; // drop signal
}

inline.@drop = inl (ret_val) {
	ret_val.p = 10;
	p("previous return value: " + ret_val);
	p("override to 100");
	drop 100; // change return value
}

inline.@drop.@drop = { |ret_val|
	ret_val.p = 10;
	p("previous return value: " + ret_val);
	p("I'm NOT overriding to 1000");
	1000; // change return value
}

a = {
	a: 10,
	b: inline()
}
p("final value(100?): " + a.b)

/*try = fn (block, args...) {
	let final = fn () {}
	block.@throw = fn () { where(); drop }
	for (let i = 0, i < args.size(), i++) {
		if (typename(args[i]) == "string") {
			if (args[i] == "catch" && i + 1 < args.size()) {
				block.@throw = (args[i + 1] << block.@throw);
				continue i++;
			} else {
				if (args[i] == "final" && i + 1 < args.size()) {
					final = args[i + 1];
					continue i++;
				}
			}
		}
	}
	block();
	final();
}*/

a = "hello?"
obj = {
	[a] : "papa"
}

delete obj.hello?
p(obj.hello?);

//import json

p(json.encode(
{
	hello? : "wow!",
	["haha"] : "eh..",
	yes : ["I'm elem1", "I'm elem2"],
	'I shouldn\'t appear' : fn () {},
	'is it null?' : null
}));
p(json.encode("this is a string"));
p(json.encode(undefined));
p(-10.1);
//json.decode("{\"hello\" : \"how are you?\\\"\", \"hey\" : -10.1 }");
debug(json.decode("{\"hi\":\"hi\", \"a\" : \"o\"}"));
debug(json.decode("{\"hi\" : null}"));
debug(json.decode("[\"hi\"]"));
//debug(json.decode("[\"I'm elem1\"]"));

p(json.encode(json.decode(json.encode(
{
	hello? : "wow!",
	["haha"] : "eh..",
	yes : ["I'm elem1", "I'm elem2"],
	'I shouldn\'t appear' : fn () {},
	'is it null?' : null
}))));

p(json.encode(json.decode("


{
	\"glossary\": {
		\"title\": \"example glossary\",
		\"GlossDiv\": {
			\"title\": \"S\",
			\"GlossList\": {
				\"GlossEntry\": {
				\"ID\": \"SGML\",
				\"SortAs\": \"SGML\",
				\"GlossTerm\": \"Standard Generalized Markup Language\",
				\"Acronym\": \"SGML\",
				\"Abbrev\": \"ISO 8879:1986\",
				\"GlossDef\": {
					\"para\": \"A meta-markup language, used to create markup languages such as DocBook.\",
					\"GlossSeeAlso\": [
							\"GML\",
							\"XML\"
						]
					},
				\"GlossSee\": \"markup\"
				}
			}
		}
	}
}


")));

p(json.decode(json.encode("\"\"\\n\"\"")));

obj = {
	to_str: fn () {
		retn "cannot turn myself to string"
	}	
};

p("" + obj);
p(1.to_str());

test_if = inl (cond, args...) {
	let i = 0;
	if (cond) {
		if (typename(args[i]) == "function") {
			args[i]();
		}
	} else {
		for (i++, i < args.size(), i++) {
			if (args[i] == "else") {
				if (args[i + 1] == "if" && typename(args[i + 2]) == "array") {
					if (args[i + 2][0]) {
						if (typename(args[i + 3]) == "function") {
							args[i + 3]();
						}
						break;
					}
				} else {
					if (typename(args[i + 1]) == "function") {
						args[i + 1]();
						break;
					}
				}
			}
		}
	}
}

test_if(0) {
	p("haha");
} else {
	p("yes!!");
} else if (1) {
	p("no!!!!wrong!!");
}

cond1 = 0;
cond2 = 0;
cond3 = 1;
// new if
if (cond1) {
	p("cond1 is true");
} else if (cond2) {
	p("cond2 is true");
} else if (cond3) {
	p("cond3 is true");
}

if (cond1) else if (cond2) {
	p("cond1 is not true while cond2 is true");
} else if (cond3) {
	p("cond1 is not true while cond3 is true");
}

if (cond1, "else") // warning: if function ended with else
if (cond1, "else", "if") // warning: if function ended with else if(requires condition and block)
if (cond1, "else", "if", new Array()) // warning: condition for else if missing
if (cond1, "else", "if", new Array("hi")) // warning: if function ended with else if(requires block)

if (cond1, "else", "if", new Array(null), fn(){}, "else", "if")
// warning: if function ended with else if(requires condition and block)

if (cond) // no warning

// switch implementation

switch = inl (&cond, args...) {
	for (let i = 0, i < args.size(), null) {
		if (args[i] == "case" &&
			typename(args[i + 1]) == "array" &&
			typename(args[i + 2]) == "function") {
			if (args[i + 1][0] == cond()) {
				args[i + 2]();
			}
		} else if (args[i] == "default" &&
				   typename(args[i + 1]) == "function") {
			args[i + 1]();
			break;
		}
		i = i + 3;
	}
}

cond = 10;
switch (cond) \
case (5) {
  	p("cond is 5");
  	// break;
} case (10) {
	p("cond is 10");
	// break; // fallthrough!!
} default {
	p("default: cond is " + cond);
}

p("all ended");

//local_val = 10;
func = fn () {
	//local_val = 20;
	retn fn () {
		p(local_val);
	}
}

(f = func())();								// (undefined)
f::local_val = "haha, it's been changed";
f()											// "haha, it's been changed"
p(local_val);								// (undefined)

p((start, 20, 30));


f::(
	local_val = "changed again!!",
	p("hello")
)											// "hello?"
f()											// "changed again!!"


f1 = fn () {
	p("1");
	yield null
	p("8");
}
f2 = fn () {
	p("2");
	if (1) {
		yield null
		p("9");
	}
}

f3 = fn () {
	cocall(f4, [], f5, []);
	p("7");
	if (1) {
		yield null
		p("10");
	}
}

f4 = fn () {
	p("3");
	if (1) {
		yield null
		p("5");
	}
}

f5 = fn () {
	p("4");
	if (1) {
		yield null
		p("6");
	}
}

i = 1;
while (0) {
	p("run " + i++);
	cocall(f1, [], f2, [], f3, []);
}
//cocall(f1, [], f2, [], f3, []);

consumer = fn (n) {
	let i = 0;
	while (i < n) {
		p(yield null);
		i++
	}
}

producer = fn (n) {
	let a = 0;
	let b = 1;
	let tmp;
	let i = 0;

	while (i < n) {
		tmp = b
		b = a + b;
		a = tmp;

		yield tmp;
		i++
	}
}

fib_async = fn (n) {
	cocall(consumer, [n], producer, [n]);
}

//fib_async(1476);

func = fn (&a...) {
	p(typename(a[0]));
	p(typename(a[1]));
}

func("no s**t", "wooo");

Coroutine = fn (&args...) {
	this.routines = new Array();

	let i = 0;
	p("haha");
	while (i < args.size()) {
		p(typename(args[i]))
		this.routines.push(args[i++]);
	}

	this.start = fn () {
		let routines = new Array();
		let i = 0;

		while (i < base.routines.size()) {
			routines.push(base.routines[i++]);
			routines.push([]);
		}
		cocall() with routines;
	}

	this.add = fn (&exp) {
		base.routines.push(exp);
	}
}

cor = new Coroutine(f1(), f2());
cor.add(f3());
// cor.start();

echo = fn () {
	let text = yield null
	p("received: " + text);
	yield text
}

sender = fn () {
	p(yield "I'm text");
}

// cocall(echo, [], sender, []);

p("################ actor test ################");

import multink

p(typename(receive.'->'));

try_actor = fn () {
	actor1 = actor () {
		import multink

		p("my name is " + actor_self());

		let try = 0

		while (1) {
			let msg = {};
			while (!(msg = receive())) {
				/*if (actor_count() == 1) {
					try++
					if (try >= 10) {
						retn
					}
				}*/
			}
			p(msg.msg);
			if (msg.msg == "stop") {
				retn;
			}
			// p("echo running");
		}
	}

	fib_async = actor (total, mod) {
		import multink
		import io

		p("my name is " + actor_self());

		let a = bignum(0);
		let b = bignum(1);
		let tmp;
		let i = 1;
		let n = 200;
		let dest = mod;

		while (i <= 5) {
			tmp = b
			b = a + b;
			a = tmp;
			if (i == dest) {
				send("" + i + "th: " + tmp.to_str()) -> "echo";
				dest = dest + total
			}
			i++
		}
		p("end!");
	}

	actor1("echo");

	fib_async("worker1", 2, 1);
	fib_async("worker2", 2, 2);
	//fib_async("worker3", 6, 3);
	//fib_async("worker4", 6, 4);
	//fib_async("worker5", 6, 5);
	//fib_async("worker6", 6, 6);

	//p(actor_count());

	join_all_but("echo");

	p("stopping echo");

	send("stop") -> "echo"
	p("all ended");

	//join_all();
}

let i = 1;
while (i <= 1) {
	p(i++)
	//try_actor();
	//join_all();
}

try_arg = actor (arg1, argv...) {
	//p("I got a argument: " + json.encode(arg1));
	debug(arg1);
	debug(argv);
	argv[1]("ha");
	//p("Variant argument size: " + argv.size());
}

a = "hello?";

let i = 1;

while (i <= 0) {
	p(i++);
	try_arg("new_actor", { a : ["s", "a"] }, "ho", fn (b) {
		p(a)
	});
	join_all();
}

p("haha");

c = {}
b = {}
a = {}
a.prototype = b
b.prototype = c
c.prototype = a

p(a.what?);

actor1 = actor () {
	import multink

	// 1 = 2;
	send("hello, root") -> "root"
	// 1 = 2;
}

actor1(_)("he");

let msg
msg = receive() for(800)
if (!msg) {
	p("no message received in 0.8 sec")
}
p(msg.msg);
p(msg.sender);

join_all();

actor1 = actor () {
	
}

//while(1)
/*
let i = 0;
while (i < 10000) {
	actor1("sub" + i);
	p("created: sub" + i++);
}*/

p((bignum("1321323213.312323132132133") + bignum("1321323213000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000000000000000000000312323132132133")).to_str());

// p(bignum());

p((bignum("90") + bignum("90")).to_str());

p("####################### BIG NUM #######################");

p((bignum("213.22") + bignum("213.22")).to_str());
p((bignum("-0.0000000002") + bignum("213.22")).to_str());
p((bignum("-0.0000000002") - bignum("213.22")).to_str());
p((bignum("-0.0000000002") - bignum("-21323.22")).to_str());
p((bignum("-0.0000000002") * bignum("213.22")).to_str());
p((bignum("-0.05") * bignum("213.2143432424")).to_str());
p((bignum("-0.05") / bignum("213.2143432424")).to_str());
p((bignum("-0.05").div(bignum("3.234324324343242498934054540594305430"), 100)).to_str());

p((-22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222.333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333).to_str());

p((bignum("213213.2222") > 213213).to_str());
p((bignum("213213.2222") < 213213).to_str());
p((bignum("213213.2222") != 213213).to_str());
p((bignum("213213.2222") == bignum("213213.222200")).to_str());
p((bignum("213213.2222") == 213213).to_str());

p((bignum("412.00000") % bignum("20.213")).to_str());
p((bignum("412") % bignum("24")).to_str());

ARGV.each() { | elem |
	stdout.puts(elem + " ");
}
(if (ARGV.size()) { p } else { fn(a){} })("");


TypeA = fn () {
	this.a = 10
	this.print = fn () {
		base.a;
	}
}

TypeB = fn () {
	this = new TypeA();
	this.a = 20;
}

a = new TypeB();
p(a print());

// BG
What = {
	's Rod': {
		s: {
			Chinese: fn () {
				let ret = {}
				ret.getter("name?") {
					p("HaHa, I dont't want to tell you");
				}
				ret
			}()
		}
	}
}

Say = {
	no: {
		to: {
			missing: fn (name) {
				p("no!! " + name);
			}
		}
	}
}
You = {
	got: {
		to: {
			be: {
				kidding: {
					me: fn () {
						let ret = { }
						ret.getter() Rod? {
							p("Yes, of course");
						}
						ret
					} ()
				}
			}
		}
	}
}
Can = {
	you: {
		play: {
			missing: fn (name) {
				if (name[-1] == "?") {
					name = name.substr(0, name.length() - 1);
				}
				switch (name) \
				case ("football") {
					p("Yes I can play " + name);
					break;
				} case ("badminton") {
					p("Yes I can play " + name);
					break;
				} default {
					p("No, I can't play " + name);
				}
			}
		}
	},
	I: {
		borrow: {
			your: {
				missing: fn (name) {
					while (name[-1] == "?") {
						name = name.substr(0, name.length() - 1);
					}
					switch (name) \
					case ("pencil") {
						p("Yes you can have my " + name);
						break;
					} case ("girlfriend") {
						p("HELL NO!!");
						break;
					} default {
						p("No, I'm afraid I can't lend you my " + name);
					}
				}
			}
		}
	}
}
// BG

What's Rod's Chinese name?
Say no to John
Say no to Nick
Say no to 'My Dear Teacher'
You got to be kidding me. Rod?
Can you play basketball?
Can you play football?
Can you play badminton?
Can I borrow your girlfriend?
Can I borrow your girlfriend??
Can I borrow your pencil?
Can I borrow your homework?

numeric_sub = $numeric.'-';
$numeric.'-' = fn (op) {
	let ret = base.to_str() + " " + op
	ret.'-' = fn (op) {
		let ret = base + " " + op;
		ret.'-' = this;
		ret
	}
	ret;
}

p(182-1589-9452)

$numeric.'-' = numeric_sub

p("######################## namespace ########################");

namespace = {
	missing: fn (name) {
		ret = top[name] = fn () { }
		delete name
		ret
	}
}

namespace a::(
	let haha,
	let hello = "hi"
)

haha = "You cannot find me"
a::haha = "I'm inside namespace"
p(haha);
p(a::haha);
p(a::hello);
p(if (a::name) { "oh no!" } else { "yes!!" });

a = 20;
nobody = "origin"
fn () {
	let a = 10;
	a
} () = "no!!"

p(nobody);

a = 20;
fn () {
	let c = 20;
	let b = 10;
	a -> c
	10
} ()

p(a);

a.p = fn (block) {
	block()
}

a.p {
	p("hello");
}

a.p with [fn () { p("ha") }]

p("######## ruby-like function definition ########");

$def_func = fn (name, fblock) {
	top[name] = fblock
}

def = {
	missing: fn (name) {
		$def_func(name, _)
	}
}

def a { | name |
	p("amazing!! --" + name);
}

a("Rod");

p(bignum("10") / 0);

$do = inl (block, &argv...) {
	let cond = null;
	let cond_i = 0;

	for (let i = 0, i < argv.size(), i++) {
		if (argv[i]() == "while") {
			if (i + 1 < argv.size()) {
				cond_i = i + 1;
				cond = fn () { argv[cond_i]()[0] };
			}
		}
	}

	block();
	if (cond) {
		while (cond()) {
			block();
		}
	}
}

$do.@throw = fn () {
	p("AH... something wrong..");
	drop
}

let i = 0;
$do {
	p(i++);
} while (i  < 10)


p((let obj = json.decode("{ \"i\": true, \"a\": false }")).i);
p(obj.a);

p("################ try/catch test ################");

error_func = fn () {
	let a.b.c.d.e.f.g = 10;
}

a = "origin"

try {
	a = error_func();
} catch { | err_msg |
	p("Error Message: " + err_msg.msg);
	p("a = " + a);
	error_func = fn () { "right!!!" }
	retry
} final {
	p("finally!!");
	p("a = " + a);
}

import io

dir = new Directory("new_dir");
if (dir.exist()) {
	p("freak!!!");
} else {
	p(dir.create());
	p("exactly!!!");
}

rmdir("new_dir");

p("##################### children of core dir #####################");
(new Directory("../core")).each("file: " + _).each { | val |
	p(val);
}

_.missing = fn (name) {
	fn (args...) {
		let ret = fn (b) {
			b[name]() with args
		}
		ret.missing = fn (name) {
			let tmp_base = base
			fn (args...) {
				let ret = fn (b) {
					tmp_base(b)[name]() with args;
				}
				ret.missing = tmp_base.missing
				ret
			}
		}
		ret
	}
}

p((_ + 1)(10));

p_all = fn (argv...) {
	argv.each { | val |
		stdout.puts(val.to_str());
	}
	p("");
}

p_for_each = _.each(p_all("from each: ", _));
p_for_each([1, 2, 3]);

x = _

formular1 = _ * 2 + 1

for (let i = 0, i < 10, i++) {
	p("when x = " + i + ", formular1 = " + formular1(i));
}

test_func = fn (first, &args...) {
	p(first)
	for (let i = 0, i < args.size(), i++) {
		p(args[i]());
	}
}

test_func(_)(123, 32434);

f = x * x

f.'[]' = fn (x) {
	tmp = base
	while (typename(tmp) == "function") {
		tmp = tmp(x)
	}
	tmp
}

$function.reconstruct = fn (context) {
	context::(fn (exps) { exps.rebuild() })(base.exp())
	// rebuild in the dest context
}

test_func = fn (func) {
	let a = "this is a"
	let b = "this is b"
	func.reconstruct(fn(){} /* capture current context */)();
}

let a = "what?"
test_func() {
	p(a)
	p(b)
}

test_func = fn (mac) {
	let a = 10
	let b = "hi"
	mac()
}

test_func(macro(){ p(a);p(b) })

regsig("hello")
regsig("yes")

test_func = fn () {
	:hello 132
	p("continue!!");
}

test_func@hello = fn (val) {
	p("hello signal, value = " + val);
}

test_func();

@yes = fn (val) {
	p("yes received");
}

:yes

@exit = fn (val) {
	p("exiting!!");
	// exit
}

exit

p("not end!!");

TypeA = fn () {
	let this.a = 10
	let this.p = fn () {
		p(base.a);
	}
}

TypeB = fn () {
	let this.a = 20
}
TypeB.prototype = new TypeA();

tmp = new TypeB();
tmp.p();

ExArray = fn () {
	this.a = 10
}
ExArray.prototype = new Array()
tmp = new ExArray();