func = { | a, b, block, block2 |
	p(a + b);
	block(a + b);
	block2();
};

func(1, 2) { | c |
	p("from block, " + c);
}{
	p("from block2, ");
};

func = (bool) {
	if (bool) {
		p("true!!!");
		return;
		p("what?");
	} {
		p("false!!!");
		return;
	};
	p("what?");
};

func(0);

func = (bool) {
	if (bool) {
		if (bool + 1) {
			return 1;
		} {
			return 2;
		};
		p(1111);
	};
	return 0;
};

p("result: " + func(-1));
p("hello, world");