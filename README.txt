Hello!

This is Wcalc 0.3.

Wcalc started as a project written for my C++ class at Ohio University (CS 240B), way back on January 14th, 2000. I made it to be a "natural language" calculator - in that it would successfully interpret any valid mathematical expression. As you can imagine, being a beginner programmer, the code was rather heinous, to say the least!

I reworked it after learning lex and yacc (powerful string parsing tools for C programmers - if you're a C programmer and you don't know them... go google!) in November 2001. Then I thought to myself - I have MacOS X, the standard calculator leaves something to be desired, and they say Cocoa programming is easy... what the heck, I'll give it a shot. Wcalc has turned out better than I thought it would - so I'm putting it on the web, for you! (If you're curious, yes, Cocoa is as wonderful as they say.)

Wcalc takes "expressions" and calculates the answer. My goal was to make the expressions relatively intutive - no reverse polish, or anything like that. So, by way of a record, here is what Wcalc can do:

Wcalc recognizes the following symbols:
+ (plus)
- (minus)
* (multiply)
/ (divide)
% (modulo (read: the remainder when integers are divided))
! (factorial)
^ (exponent. four squared is 4^2)
> (less than)
< (greater than)
<= (less than or equal to)
>= (greater than or equal to)
== (equal)
!= (not equal)
&& (logical and)
|| (logical or)

Wcalc also allows you to group operations with parenthesis (), brackets [], and braces {}.

Wcalc supports the following functions:
sin cos tan asin acos atan sinh cosh tanh asinh acosh atanh
log ln

Wcalc has a few built-in constants:
pi e

Wcalc supports user-defined variables. You define a variable like this:
mynewvariable=5
Then you can use it in an expression like this: 1+3*mynewvariable/4

A few variable names are already taken, and have special meaning.
E - indicates whether or not to use engineering notation or not. If the precision slider is at 0, Wcalc may use engineering notation if it's a long answer, but for specific precisions, it will only use engineering notation if this variable is "1". E cannot be used in expressions.
a - this variable represents the last answer, and may be used in expressions.
P - this variable stores the current precision (you can set it with the slider), and may not be used in expressions (this one is ripe for removal).
q - this variable is reserved in the command-line version (it means quit), and cannot be used in expressions.
M - this letter is a holdover from before there was proper variable support, and in the command-line version, it stores the last answer in the "m" variable (this one is ripe for removal too).
m - described above.

*************
Installation
*************
Installing the regular version of Wcalc is easy - just drag the application to your disk (the Applications folder is a good place to put it). Installing the command-line version of wcalc involves copying it to /usr/bin (which requires root privileges) - with the command "sudo cp wcalc /usr/bin/wcalc". This will become easier (read: a package) soon.