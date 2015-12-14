

func = fn (b) {
	let a = "haha";
	i = a;
	b()
	a.b();
}

a = 0;
i = 110;
func() {
	i.b = fn () { }
	i = 0;
}