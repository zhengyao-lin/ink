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

<br>

##1. Getting Start
This part will tell you how to install ink interpreter in your own computer.

#####If your operating system is...


####1.1 Linux Distribution on x86 / Mac OS X
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
(Lower versions may work, but I haven't tested them)

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

####1.2 Windows on x86
#####First, create a temp dir and clone the source code using git
if you haven't installed git on your Windows, you can download zip file from [GitHub](https://github.com/Rod-Lin/ink "ink") directly

	mkdir ink-src && cd ink-src
	git clone https://github.com/Rod-Lin/ink.git
	
#####Secondly, install [MinGW](http://www.mingw.org, "MinGW")(requires base package, g++ bin package, msys bison package and msys flex package)
Before compiling, make link yacc and lex to bison and flex

	ln -s /bin/bison /bin/yacc
	ln -s /bin/flex /bin/lex
	
#####Then, cd in source dir and compile with the following command

	cd ink
	make PLATFORM=win64
	make output

#####All executable file will be in output, you can run example code for test

	./output/ink block.ik


####1.3 Linux on ARM
Similar process. Install g++ cross-compiler in the host(with Linux on x86) first, and make with argument ARCH=arm

	make ARCH=arm

After compiling, copy the folder "bin" and "lib" into your ARM device(normally on /usr) with scp or anything you like.

<br>

##2. Programming with ink
Specific details are under editing. Comming soon.

<br>

##3. Contact me
 Rod L: lancelot.b@outlook.com
