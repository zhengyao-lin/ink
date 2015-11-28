fib = (n) {
	if (n) {
		if (n - 1) {
			return fib(n - 2) + fib(n - 1);
		};
	};
	1;
};

p(fib(22));

/*
func = () {
	if (1) {
		if (1) {
			p("hi");
			return 10;
			p("shit");
		};
	};
};
p(func());*/