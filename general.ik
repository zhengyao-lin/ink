p(10 + 20);

a = 10;
func = fn () {
	a = 20;
	p(a);
};
func();

func = fn (a) { a; };

func2 = fn () {};

func2.p = fn (a) {
	func(a);	
};

fn (f) {
	f.p = fn (a) { 0; };
} (func2);

p(func2.p(120));

g1 = 10;

fn (a ,
b
)
{
if (
	

	10 					==
				10
) {
	p    	(
	" w o w
	"
	) ;


}

} (
"s",  			
				"b"
);