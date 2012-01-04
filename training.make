#training.make

training.out:main.o unary.o relation.o control_flow.o
	g++ -o training.out main.o unary.o relation.o control_flow.o

main.o: ./source/main.c ./header/unary.h
	g++ -c -Wall ./source/main.c -I header

unary.o: ./source/unary.c ./header/unary.h
	g++ -c -Wall ./source/unary.c -I header

relation.o: ./source/relation.c ./header/relation.h
	g++ -c -Wall ./source/relation.c -I header

control_flow.o: ./source/control_flow.c ./header/control_flow.h
	g++ -c -Wall ./source/control_flow.c -I header

clean:
	rm *.o training.out

#end of make
