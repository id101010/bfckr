# bfckr
A little brainfuck interpreter written in C.

# How to run
* Type "make"
* Read [this](https://en.wikipedia.org/wiki/Brainf**k)
* Create your own brainfuck software
* ./bfckr "$yourstuff"

# Usage
Usage: ./bfckr [OPTION] [FILE]
-h        Show this help.
-d        Enable debugger.
-f         Execute brainfuck code given as file.
-e        Execute brainfuck code given as argument.

## examples:

./bfckr -f examples/pi.bf
~~~~
3.14070455282885
~~~~

./bfckr -d -f examples/pi.bf
~~~~
[ENTER]: single step [c]: continue
Source viewer:                                            
------------------------------------------------------------
                              >+++++++++++++++[<+>>>>>>>>+++
                              ^                             
                              ip=0                         
------------------------------------------------------------

Memory viewer:                                            
------------------------------------------------------------
000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 

                              ^                             
                              mp=1                         
000 000 000 000 000 000 001 002 003 004 005 006 007 008 009 
------------------------------------------------------------
~~~~

./bfckr -e "----[---->+<]>++."
~~~~
A
~~~~

# Breakpoints

A # in the brainfuck source will be interpreted as breakpoint. You can use them if you want to stop the program flow at a specific point and start the debugger from there. If you are in debugger mode pressing c will get you back to executing the program.

./bfckr -e "----[#---->+<]>++."
~~~~
[ENTER]: single step [c]: continue

Source viewer:                                            
------------------------------------------------------------
                         ----[#---->+<]>++.
                              ^                             
                              ip=5                         
------------------------------------------------------------

Memory viewer:                                            
------------------------------------------------------------
000 000 000 000 000 000 000 -04 000 000 000 000 000 000 000 
                              ^                             
                              mp=0                         
993 994 995 996 997 998 999 000 001 002 003 004 005 006 007 
------------------------------------------------------------
~~~~

# Licence

![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/logo-220x1601.png)

Copyright © 2016 Aaron aaron@duckpond.ch
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See the COPYING file for more details.
