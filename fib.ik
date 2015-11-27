fib = (n) {
	if (n) {
		if (n - 1) {
			//p("hi");
			return (fib(n - 2) + fib(n - 1));
			//p("no");
		};
		//p("wow");
	};
	//p("e");
	1;
};

p(fib(10));

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