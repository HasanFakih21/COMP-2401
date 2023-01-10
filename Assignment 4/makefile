all: prog1 prog2

prog1: tester display planner
	gcc -o Tester plannerTester.o display.o pathPlanner.o -lX11
prog2: bigtester display planner
	gcc -o bigTester bigEnvironment.o display.o pathPlanner.o -lX11
tester: plannerTester.c display.h obstacles.h
	gcc -c plannerTester.c
display: display.c display.h obstacles.h
	gcc -c display.c
planner: pathPlanner.c display.h obstacles.h
	gcc -c pathPlanner.c
bigtester: bigEnvironment.c display.h obstacles.h
	gcc -c bigEnvironment.c
clean:
	rm -f plannerTester.o display.o pathPlanner.o bigEnvironment.o Tester bigTester
