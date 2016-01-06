io.load(io)

/*p("contents\n");
src = new File("block.ik");
p(src.read());
p("end\n");

src.close();*/


while (1) {
	stdout.puts(stdin.getc());
}

fun = fn (n, o) {
  p(o)
  retn {
    fun: fn (m) {
      retn fun(m,n);
    }
  };
}
var a = fun(0);  a.fun(1);  a.fun(2);  a.fun(3);
var b = fun(0).fun(1).fun(2).fun(3);
var c = fun(0).fun(1);  c.fun(2);  c.fun(3);

Promise = fn (solve) {
	retn {
		then: fn (args...) {
			p(args.size());
			solve() with args
			retn base
		}
	}
}

a = new Promise(fn (func) {
	func();
})

a.then({ | |
	p("hahasss");
}).then() {
	p("hahaiii");
}