
Main: Main.o FirstPass.o SecondPass.o Data.o Macros.o Utills.o Parser.o
	gcc -g -Wall -ansi -pedantic Main.o Utills.o Parser.o FirstPass.o SecondPass.o Data.o Macros.o  -o Main

Main.o: Main.c Data.h Utills.h Macros.h FirstPass.h SecondPass.h
	gcc -c -g -Wall -ansi -pedantic Main.c -o Main.o

Macros.o: Macros.c Parser.h Utills.h Macros.h Data.h
	gcc -c -g -Wall -ansi -pedantic Macros.c -o Macros.o

Parser.o: Parser.c Parser.h Utills.h Data.h
	gcc -c -g -Wall -ansi -pedantic Parser.c -o Parser.o

FirstPass.o: FirstPass.c Data.h FirstPass.h Parser.h
	gcc -c -g -Wall -ansi -pedantic FirstPass.c -o FirstPass.o

SecondPass.o: SecondPass.c Data.h Parser.h Utills.h
	gcc -c -g -Wall -ansi -pedantic SecondPass.c -o SecondPass.o

Data.o: Data.c Data.h Utills.h
	gcc -c -g -Wall -ansi -pedantic Data.c -o Data.o

Utills.o: Utills.c Data.h Utills.h
	gcc -c -g -Wall -ansi -pedantic Utills.c -o Utills.o


