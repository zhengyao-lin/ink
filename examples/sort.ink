#! /usr/bin/ink

import blueprint
import blueprint.math
import io

$array.p = fn () {
	let str = ""
	base.each { | v |
		if (str != ""):
			str += " "

		str += v
	}
	p(str)
}

let isort = fn (list) { /* in ascending order */
	for (let i = 0, i < list.size(), i++) {
		let key = list[i]
		for (let j = i, j > 0, j--) {
			if (list[j - 1] > key) {
				list[j] = list[j - 1]
			} else {
				break
			}
		}
		list[j] = key
	}
	list
}

let mgsort_merge = fn (l1, l2) {
	let size = l1.size() + l2.size()
	let n = 0
	let m = 0
	let l1s = l1.size()
	let l2s = l2.size()
	let ret = []

	for (let i = 0, i < size, i++) {
		if (m < l2s && n < l1s) {
			if (l1[n] > l2[m]) {
				ret.push(l2[m++])
			} else {
				ret.push(l1[n++])
			}
		} else {
			if (m < l2s) {
				ret.push(l2[m++])
			} else {
				ret.push(l1[n++])
			}
		}
	}
	ret
}

let mgsort = fn (list) {
	if (list.size() <= 2) {
		if (list.size() == 2 && list[0] > list[1]) {
			let tmp = list[0]
			list[0] = list[1]
			list[1] = tmp
		}
		retn list
	}

	//p(list.size())

	let split = (list.size() / 2).floor()

	mgsort_merge(
		this(list[0, split]),
		this(list[split + 1, -1])
	)
}

let qsort_sub = fn (list, i, j) {
	let r1 = math.random(i, j + 1).floor()
	let r2 = math.random(i, j + 1).floor()
	let r3 = math.random(i, j + 1).floor()
	let fi = -1

	if (list[r1] >= list[r2]) {
		if (list[r2] >= list[r3]) {
			fi = r2
		} else {
			if (list[r1] >= list[r3]) {
				fi = r3
			} else {
				fi = r1
			}
		}
	} else {
		if (list[r2] <= list[r3]) {
			fi = r2
		} else {
			if (list[r1] >= list[r3]) {
				fi = r1
			} else {
				fi = r3
			}
		}
	}

	let key = list[fi]
	let m = i - 1, n = j + 1
	let tmp = null

	while (1) {
		m++
		while (list[m] < key) {
			m++
		}

		n--
		while (list[n] > key) {
			n--
		}

		if (m >= n) {
			break
		}

		tmp = list[m]
		list[m] = list[n]
		list[n] = tmp
	}

	if (i < n) {
		qsort_sub(list, i, n)
	}
	if (n + 1 < j) {
		qsort_sub(list, n + 1, j)
	}

	list
}

let qsort = fn (list) {
	qsort_sub(list, 0, list.size() - 1)
}

let ctsort = fn (list) { /* element less than 10001 */
	let max = 100
	list.each { | v |
		if (v > max) {
			max = v
		}
	}

	let ct = new Array(max + 1, 0)

	list.each { | v |
		ct[v]++
	}

	let ret = []
	let num = 0

	ct.each { | v |
		ret += new Array(v, num++)
	}

	ret
}

isort([0, 22, 53, 32, 16, 72, 3, 60]).p()
mgsort([0, 22, 53, 32, 16, 72, 3, 60]).p()
qsort([0, 22, 53, 32, 16, 72, 3, 60]).p()
ctsort(200.times()[,,-1]).p()
