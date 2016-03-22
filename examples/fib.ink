import blueprint
//import bignums

//dp = new Array(101)

/* import "test.ik"
import "block.ik"
import "general.ik"
*/

fib = fn (n) : retn ((n == 0 || n == 1) || (self(n - 2) + self(n - 1)))

p(fib(15).to_str());
