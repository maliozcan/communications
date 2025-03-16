THIRD_PARTY_DIR=third_party
INC_DIR=include
TEST_DIR=test
SIM_DIR=simulation
MISC_DIR=misc
CPP = clang++
CPPFLAGS = -std=c++17 -g -Wall -Wextra -Wpedantic  -Werror
LDLIBS=-lfftw3

# if you have curl, use it, otherwise try wget.

.PHONY: clean

all: dependencies test simulation misc

# Dependencies
$(THIRD_PARTY_DIR)/doctest.h:
		mkdir -p $(THIRD_PARTY_DIR)
		curl https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -o $(THIRD_PARTY_DIR)/doctest.h

dependencies: $(THIRD_PARTY_DIR)/doctest.h

# Tests
test: $(THIRD_PARTY_DIR)/doctest.h $(TEST_DIR)/test.cpp $(TEST_DIR)/psk_test.o
		@echo $(CPP) "$<"
		@echo "linking $@"
		$(CPP) $(CPPFLAGS) -I$(THIRD_PARTY_DIR) $(TEST_DIR)/psk_test.o -o $(TEST_DIR)/test $(TEST_DIR)/test.cpp

$(TEST_DIR)/psk_test.o: $(TEST_DIR)/psk_test.cpp $(INC_DIR)/psk.hpp
		@echo $(CPP) "$<"
		$(CPP) $(CPPFLAGS) -I$(INC_DIR) -I$(THIRD_PARTY_DIR) -c $(TEST_DIR)/psk_test.cpp -o $(TEST_DIR)/psk_test.o


# Simulation
simulation: $(SIM_DIR)/bpsk_simulation $(SIM_DIR)/qpsk_simulation

$(SIM_DIR)/bpsk_simulation: $(SIM_DIR)/bpsk_simulation.cpp $(INC_DIR)/psk.hpp $(INC_DIR)/gplot.h $(INC_DIR)/utilities.hpp
		@echo $(CPP) "$<"
		$(CPP) $(CPPFLAGS) -I$(INC_DIR) -O3 -o $(SIM_DIR)/bpsk_simulation $(SIM_DIR)/bpsk_simulation.cpp

$(SIM_DIR)/qpsk_simulation: $(SIM_DIR)/qpsk_simulation.cpp $(INC_DIR)/psk.hpp $(INC_DIR)/gplot.h
		@echo $(CPP) "$<"
		$(CPP) $(CPPFLAGS) -I$(INC_DIR)  -o $(SIM_DIR)/qpsk_simulation $(SIM_DIR)/qpsk_simulation.cpp


misc: $(MISC_DIR)/fft-example

$(MISC_DIR)/fft-example: $(MISC_DIR)/fft-example.cpp
		@echo $(CPP) "$<"
		$(CPP) $(CPPFLAGS) -I$(INC_DIR) -O3 -o $(MISC_DIR)/fft-example $(MISC_DIR)/fft-example.cpp $(LDLIBS)

# Utilities
clean:
		rm -rf *.o $(TEST_DIR)/*.o $(TEST_DIR)/test $(SIM_DIR)/*_simulation $(MISC_DIR)/fft-example

$(VERBOSE).SILENT:


# $(THIRD_PARTY_DIR)/gnuplot_i.hpp:
# 		mkdir -p $(THIRD_PARTY_DIR)
# 		mkdir -p $(TMP_DIR)
# 		curl https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/gnuplot-cpp/gnuplot-cpp.zip -o $(TMP_DIR)/gnuplot-cpp.zip
# 		unzip $(TMP_DIR)/gnuplot-cpp.zip -d $(TMP_DIR)/
# 		cp $(TMP_DIR)/gnuplot-cpp/gnuplot_i.hpp $(THIRD_PARTY_DIR)/gnuplot_i.hpp
# 		rm -rf $(TMP_DIR)

