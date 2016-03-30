#! /usr/bin/ink

import blueprint

$array.find = fn (elem) {
	let ret = 0
	base.each { | v |
		if (v == elem): ret++
	}
	ret
}

$array.to_str = fn () {
	let out = ""
	base.each { | v |
		if (out != ""): out += ", "

		out += v.to_str()
	}
	out
}

let Action = fn (from, to, q) {
	this.from = from
	this.to = to
	this.q = q
	this.to_str = fn () {
		"(" + base.from + "->" + base.to + ", " + base.q + ")"
	}
}

let BucketState = fn (states...) {
	this = clone states

	this.checkAction = fn (action) {
		if (action.from == to): retn 0

		if (!(action.from < base.size() && action.from >= 0 &&
			action.to < base.size() && action.to >= 0)): retn 0

		if (!(action.q <= base[action.from] &&
			action.q <= (max_sizes[action.to] - base[action.to]))): retn 0

		if (action.q == base[action.from] || action.q == max_sizes[action.to] - base[action.to]):
			retn 1

		retn 0
	}

	this.applyAction = fn (action) {
		let ret = clone base
		ret[action.from] -= action.q
		ret[action.to] += action.q
		ret
	}

	this.`==` = fn (op) {
		let ret = 1
		let i = 0

		base.each { | v |
			ret &= v == op[i++]
		}

		ret
	}

	this.to_str = fn () {
		let ret = "("
		base.each { | v |
			if (ret != "("): ret += ", "

			ret += v.to_str()
		}
		retn ret + ")"
	}
}

let tryBucket = fn (hist_states, cur_state, actions) {
	try_count++
	if (if_accept(cur_state)) {
		// p(hist_states.to_str())
		solution.push(actions)
		retn
	}
	# p((++try_count).to_str() + ": " + cur_state.to_str())

	let i = 0
	cur_state.each { | from |
		let j = 0
		cur_state.each { | to |
			if (j != i) {
				for (let q = from, q > 0, q--) {
					let action = new Action(i, j, q)
					if (cur_state.checkAction(action)) {
						let n_state = cur_state.applyAction(action)
						if (!hist_states.find(n_state)) {
							tryBucket(hist_states + [n_state], n_state, actions + [action])
						}
					}
				}
			}
			j++
		}
		i++
	}
}

let max_sizes = [8, 5, 3]
let init_state = new BucketState(8, 0, 0)
let if_accept = fn (state) {
	state.find(4) >= 2
}

let solution = []
let try_count = 0

tryBucket([init_state], init_state, [])

let min_solution = null
let min_len = null

solution.each { | v |
	if (!min_len || v.size() < min_len) {
		min_solution = v
		min_len = v.size()
	}

	p(v.to_str())
}

p("found " + solution.size() + " solution(s) in total")
p("tried " + try_count + " action(s)")
p("shortest solution(" + (min_len || "N/A") + " step(s)):" + ((min_solution && "\n" + min_solution.to_str()) || " NONE"))
