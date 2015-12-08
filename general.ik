Pair = (f, s) {
	first : f;
	second : s;
	'[]' : (index) {
		if (index == 0) {
			return base.first;
		} {
			if (index == 1) {
				return base.second;
			};
		};
		return undefined;
	};
	p : () {
		p("pair: \n  first: " + base.first + "\n  second: " + base.second);
	};
};

new_pair = new Pair();
new_pair.first = "first!!";
new_pair[1] = "this is second";
new_pair.p();

sort = (list, compare) {
	let i = 0;
	let k = 1;
	let a = 0;

	while ([k]) {
		i = a;
		k = 0;
		while ([i < list.size() - 1]) {
			if (compare(list[i], list[i + 1])) {
				tmp = list[i];
				list[i] = list[i + 1];
				list[i + 1] = tmp;
				k = k + 1;
			};
			if (!k && a < i) {
				a = i;
			};
			i = i + 1;
		};
	};

	return list;
};
/*
list = new Array(new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20), new Pair(10, 20));
list.p = () {
	let i = 0;
	while ([i < base.size()]) {
		p("" + base[i].first + ", " + base[i].second);
		i = i + 1;
	};
	null;
};
list.p();
sort(list, (a, b) { return a.first > b.first; }).p();
*/
quick_sort = (list, start, end) {
    if (start < end) {
        let pivotpos = partition(list, start, end);
        quick_sort(list, start, pivotpos - 1);
        quick_sort(list, pivotpos + 1, end);
    };
};

partition = (list, start, end) { 
	let pivotpos = start; 
	let pivot = list[start]; 
	let tmp;
	let i = start + 1;
	while ([i <= end]) {
		if (list[i] < pivot) {
			tmp = list[i];
			pivotpos = pivotpos + 1;
			list[i] = list[pivotpos];
			list[pivotpos] = tmp;
		};
		i = i + 1;
	};

	tmp = list[start]; 
	list[start] = list[pivotpos]; 
	list[pivotpos] = tmp; 
	return pivotpos; 
};

     
list = new Array(8, 2); 
list.p = () {
	let i = 0;
	while ([i < base.size()]) {
		p(base[i]);
		i = i + 1;
	};
	null;
};
quick_sort(list, 0, list.size() - 1);
list.p();