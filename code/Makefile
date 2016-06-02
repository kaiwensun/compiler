g--:
	bison -d syntax.y
	flex lexical.l
	gcc main.c syntax.tab.c syntaxtree.c semantic.c translate.c iropt.c mips.c -lfl -ly -o g--
gdb:
	bison -d syntax.y
	flex lexical.l
	gcc -g main.c syntax.tab.c syntaxtree.c semantic.c translate.c iropt.c mips.c -lfl -ly -o g--
debug:
	bison -d -t syntax.y
	flex lexical.l
	gcc main.c syntax.tab.c syntaxtree.c semantic.c  translate.c iropt.c mips.c --lfl -ly -o g--
output:
	bison -d -v syntax.y
	flex lexical.l
	gcc main.c syntax.tab.c syntaxtree.c semantic.c translate.c iropt.c mips.c --lfl -ly -o g--
