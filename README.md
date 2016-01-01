ink Tutorial
===

####What is Ink?

ink is a prototype-based language, which has one interpreter currently, implemented by C++.
You can view and try it on this website.

The purpose of designing ink is to create a language that put less weight on efficiency and focus more on flexibility and grace.

So have a look of what ink looks like


	import io
	
	Promise = fn (solve) {
		retn {
			then: fn (args...) {
				solve() with args
				retn base
			}
		}
	}
	
	a = new Promise(fn (func) {
		func();
	})
	
	a.then({ | |
		stdout.puts("hello, ");
	}).then() {
		p("how are you?");
	}

Seems simple, isn't it?
The grammar of ink is designed to be simple as well as flexible, which costs a little to learn. If you have any programming experiences in C/C++, Ruby, Python or JavaScript, then it'll be quite easy to get used to ink.


##1. Getting Start
This part will tell you how to install ink interpreter in your own computer.

#####If your operating system is...


####1.1 Linux Distribution / Mac OS X
#####First, create a temp dir and clone the source code using git
	mkdir ink-src && cd ink-src
	git clone https://github.com/Rod-Lin/ink.git
<br>
#####Secondly, cd into the source dir and compile
make sure your system has the following dependencies:
	1. GNU Bison >= 3.0.2
	2. GNU Flex >= 2.5.39
	3. GNU Make >= 4.0
	4. GNU G++ >= 5.2.1
(Lower version may work, but I haven't tested them)

	cd ink
	make

If everything is alright, there will be no error.
If anything goes wrong, please check whether those dependencies have been installed correctly or you can contact me with the email below.

#####Thirdly, install
	sudo make install # make sure you are the administrator

After installing, you can run interpreter by command

	ink <source_file>

You can copy the codes in the beginning of this tutorial into a file(assume test.ik), and run it

	ink test.ik

The correct output should be

	hello, how are you?

####1.2 Windows & Others
Transplantation is under processing. Please wait tight




##2. Programming with ink
Specific details are under editing. Comming soon.



##3. Contact me
 Rod L: lancelot.b@outlook.com