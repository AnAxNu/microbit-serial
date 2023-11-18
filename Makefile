# This Makefile compiles the library into a object file and puts it in the ./lib folder
# It can also compile the test-program that will get value and display it
MBS_LIBDIR=./lib
MBS_LIBRARY_NAME=microbit-serial
MBS_LIBRARY_FILE=$(MBS_LIBDIR)/$(MBS_LIBRARY_NAME).o


all: clean $(MBS_LIBRARY_NAME) test

$(MBS_LIBRARY_NAME):$(MBS_LIBRARY_NAME).cpp
	$(CXX) -c -fPIC microbit-serial.cpp -o $(MBS_LIBRARY_FILE) -std=c++11

test:
	$(CXX) -I./include/ microbit-serial.cpp microbit-serial-test.cpp -o microbit-serial-test

clean:
	rm -f $(MBS_LIBDIR)/*.o
	rm -f ./microbit-serial-test