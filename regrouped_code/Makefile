# To compile a file with functions for reading or moving (ie everything but the move function that calls
#the sensors replace testMove by the name of your file and include the .h file you find in the regrouped_code
#directory. Note that: the update sensors value need to be included in your file that contains the main.
# Take the testMove build as exemple and it should work!!

DEPENDENCIES = readFunctions.o moveFunctions.o detect_disting_functions.o positionFunctions.o bluetoothFunctions.o sendPositionThread.o scouting.o linkedlist.o Boundaries.o
LIBRAIRIES = -lm  -lev3dev-c -lbluetooth -lpthread

mainBoundaries : mainBoundaries.c $(DEPENDENCIES) defines.h
		gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

Boundaries.o:  Boundaries.c
	gcc -lev3dev-c -lpthread -c $< -o $@
MainCompetitionBigStadium: MainCompetitionBigStadium.c $(DEPENDENCIES) defines.h
	gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

MainCompetitionSmallStadium: MainCompetitionSmallStadium.c $(DEPENDENCIES) defines.h
	gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

MainTest: MainTest.c $(DEPENDENCIES) defines.h
	gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

MainTestSendMap : MainTestSendMap.c $(DEPENDENCIES) defines.h
	gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

mainTestScout: mainTestScout.c $(DEPENDENCIES) defines.h
	gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

MainSensorsThread : MainSensorsThread.c $(DEPENDENCIES) defines.h
	gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

MainRotate : MainRotate.c $(DEPENDENCIES) defines.h
	gcc $(LIBRAIRIES) $@.c $(DEPENDENCIES) -o $@

MainComplete : MainComplete.c readFunctions.o moveFunctions.o detect_disting_functions.o positionFunctions.o bluetoothFunctions.o sendPositionThread.o
	gcc -lm  -lev3dev-c -lbluetooth -lpthread $^ -o $@

MainScout : MainScout.c readFunctions.o moveFunctions.o detect_disting_functions.o positionFunctions.o bluetoothFunctions.o sendPositionThread.o
	gcc -lm  -lev3dev-c -lbluetooth -lpthread $^ -o $@

MainRelease : MainRelease.c readFunctions.o moveFunctions.o detect_disting_functions.o positionFunctions.o bluetoothFunctions.o
	gcc -lm  -lev3dev-c -lbluetooth -lpthread $^ -o $@

MainDisting : MainDisting.c readFunctions.o moveFunctions.o detect_disting_functions.o positionFunctions.o bluetoothFunctions.o
	gcc -lm  -lev3dev-c -lbluetooth -lpthread $^ -o $@

MainTachoCount : MainTachoCount.c readFunctions.o moveFunctions.o detect_disting_functions.o  bluetoothFunctions.o positionFunctions.o
	gcc -lev3dev-c -lbluetooth $^ -o $@

readFunctions.o : readFunctions.c
	gcc -lev3dev-c -lpthread -c $< -o $@

moveFunctions.o : moveFunctions.c
	gcc -lev3dev-c -c $< -o $@

detect_disting_functions.o : detect_disting_functions.c
	gcc -lev3dev-c -c $< -o $@

bluetoothFunctions.o : bluetoothFunctions.c
	gcc -lm -lev3dev-c -lbluetooth -lpthread -c $< -o $@

positionFunctions.o : positionFunctions.c
	gcc -lev3dev-c -c $< -o $@

sendPositionThread.o : sendPositionThread.c
	gcc -lev3dev-c -lpthread -c $< -o $@

sensorsThread.o : sensorsThread.c
	gcc -lev3dev-c -lpthread -c $< -o $@

MainBlue : MainBlue.c readFunctions.o moveFunctions.o detect_disting_functions.o positionFunctions.o bluetoothFunctions.o sendPositionThread.o
	gcc -lm  -lev3dev-c -lbluetooth -lpthread $^ -o $@

scouting.o : scouting.c
	gcc -lev3dev-c -lpthread -c $< -o $@

linkedlist.o : linkedlist.c
	gcc -lev3dev-c -lpthread -c $< -o $@

run:
	./MainComplete


clean :
	rm moveFunctions.o
	rm detect_disting_functions.o
	rm readFunctions.o
	rm positionFunctions.o
	rm scouting.o
