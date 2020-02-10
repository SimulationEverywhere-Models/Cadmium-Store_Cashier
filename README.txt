This folder contains the Store Cashier   DEVS model implemented in Cadmium

/**************************/
/****FILES ORGANIZATION****/
/**************************/

README.txt
makefile

atomics [This folder contains atomic models implemented in Cadmium]
	client_generator.hpp
	employee.hpp
	queue.hpp
bin [This folder will be created automatically the first time you compile the project.
     It will contain all the executables]
build [This folder will be created automatically the first time you compile the project.
       It will contain all the build files (.o) generated during compilation]
data_structures [This folder contains message data structure used in the model]
	message.hpp
	message.cpp
simulation_results [This folder will be created automatically the first time you compile the project.
                    It will store the outputs from your simulations and tests]
test [This folder the unit test of the atomic models]
	main_client_generator.cpp
top_model [This folder contains the store queue top model]
	main.cpp
	
/*************/
/****STEPS****/
/*************/

1 - Update include path in the makefile in this folder and subfolders. You need to update the following lines:
	INCLUDECADMIUM=-I ../../cadmium/include
	INCLUDEDESTIMES=-I ../../DESTimes/include
    Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include
	Do the same for the DESTimes library
    NOTE: if you follow the step by step installation guide you will not need to update these paths.
2 - Compile the project and the tests
	1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the store_queue folder
	3 - To compile the project and the tests, type in the terminal:
			make clean; make all
3 - Run subnet test
	1 - Open the terminal in the bin folder. 
	2 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE" (For windows, "./NAME_OF_THE_COMPILED_FILE.exe"). 
	For this specific test you need to type:
			./CLIENT_GENERATOR_TEST (or ./CLIENT_GENERATOR_TEST.exe for Windows)
	3 - To check the output of the test, go to the folder simulation_results and open  "client_generator_test_output_messages.txt" and "client_generator_test_output_state.txt"
4 - To run receiver and sender tests you just need to select the proper executable name in step 3.2
			
5 - Run the top model
	1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the bin folder.
	3 - To run the model, type in the terminal "./NAME_OF_THE_COMPILED_FILE". For this test you need to type:
		./STORE (for Windows: ./STORE.exe)
	4 - To check the output of the model, go to the folder simulation_results and open "store_output_messasges.txt" and "store_output_state.txt"
	5 - To run the model with different inputs
		5.1. If you want to keep the output, rename "bank_output_messasges.txt" and "bank_output_state.txt". Otherwise it will be overwritten when you run the next simulation.
