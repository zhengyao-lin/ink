// dp = new Array(100)

/* import "test.ik"
import "block.ik"
import "general.ik"
*/

fib = fn (n) do
	/* if (dp[n]) do
		retn dp[n]
	end */
	if (n == 0 ||
		n == 1) do
		retn 1
	end
	
	retn this(n - 2) + this(n - 1)
end

p(fib(15))