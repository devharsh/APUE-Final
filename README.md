[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a5ad672f141b459cb8195febbeb6b476)](https://www.codacy.com/manual/devharsh/APUE-Final?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=devharsh/APUE-Final&amp;utm_campaign=Badge_Grade)

# APUE-Final
Stevens Institute of Technology - CS 631 - APUE - Final - Simple Shell

## NAME
sish —a simple hell

## SYNOPSIS
sish [ −x] [ −c command]

## DESCRIPTION
sish implements a very simple command-line interpreter or shell. It is suitable to be used interactively or
as a login shell. It only implements a very small subset of what would usually be expected of a Unix shell,
and does explicitly not lend itself as a scripting language.

## Tests
localhost$ ./sish -c date
Mon Dec 16 22:38:53 UTC 2019
localhost$ ./sish -c ls
Makefile  README.md checklist sish      sish.c    sish.h    sish.o
localhost$ ./sish -x
sish$ echo $$
+ echo $$
2797
sish$ echo $?
+ echo $?
0
sish$ pwd
+ pwd
/home/dev/UNIX_programs/FINAL
sish$ cd
+ cd
sish$ pwd
+ pwd
/home/dev
sish$ ls
+ ls
UNIX_programs
sish$ echo $$ > 1.txt
+ echo $$
sish$ cat < 1.txt
+ cat <
2797
