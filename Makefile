OBJ = main.o startTheGame.o
TEST_OBJ = test.o
LD = -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf
FLAGS = -Wall

Fight_Racer : $(OBJ)
	gcc $(LD) $(FLAGS) -o Fight_Racer $(OBJ)

main.o: constants.h
startTheGame.o: constants.h controls.h random.h
test.o: test.c

.PHONY : clean dir test
clean:
	rm $(OBJ)
test: $(TEST_OBJ)
	gcc $(FLAGS) -o Test $(TEST_OBJ)
