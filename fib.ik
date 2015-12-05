// dp = new Array(100);

fib = (n) {
	/*if (dp[n]) {
		return dp[n];
	};*/
	if (n == 0 || n == 1) {
		return 1;
	};
	
	return this(n - 2) + this(n - 1);
};

p(fib(15));

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