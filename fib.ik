dp = new Array(100);

fib = (n) {
	if (dp[n]) {
		return dp[n];
	};
	if (n) {
		if (n - 1) {
			return dp[n] = fib(n - 2) + fib(n - 1);
		};
	};
	dp[n] = 1;
};

p(fib(20));

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