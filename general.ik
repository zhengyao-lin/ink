for = inl (init, &cond, &iterate, block) {
	while (cond()) {
		block();
		iterate();
	}
}

max = fn (args...) {
	for (let i = 1 && let max = args[0],
		 i < args.size(), i = i + 1) {
		if (args[i] > max) { max = args[i]; }
	}
	max;
}

ExArray = fn () {
	this.prototype = new Array();
}

a = new ExArray();
p(a.size());