CXX = g++
CXXFLAGS = -Wall -std=c++11 -fPIC

BIN_DIR = .
LIB_DIR = $(BIN_DIR)
SRC_DIR = $(BIN_DIR)/src
INCLUDE = -I ../include -I ../externals/EasyBMP/include/
OBJ_DIR = $(BIN_DIR)/obj
BUILD_OBJ_DIR = ../build/obj


.DEFAULT_GOAL := all


.PHONY: all
all: $(OBJ_DIR)/*.o $(LIB_DIR)/*.so

$(OBJ_DIR)/*.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC_DIR)/autocontrast.cpp -c -o $(OBJ_DIR)/autocontrast.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC_DIR)/gray_world.cpp -c -o $(OBJ_DIR)/gray_world.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC_DIR)/median.cpp -c -o $(OBJ_DIR)/median.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC_DIR)/unsharp.cpp -c -o $(OBJ_DIR)/unsharp.o

$(LIB_DIR)/*.so:
	$(CXX) -shared -std=c++11 $(OBJ_DIR)/autocontrast.o $(BUILD_OBJ_DIR)/align.o $(BUILD_OBJ_DIR)/filters.o $(BUILD_OBJ_DIR)/plugin.o -o $(LIB_DIR)/lib_autoc.so
	$(CXX) -shared -std=c++11 $(OBJ_DIR)/gray_world.o $(BUILD_OBJ_DIR)/align.o $(BUILD_OBJ_DIR)/filters.o $(BUILD_OBJ_DIR)/plugin.o -o $(LIB_DIR)/lib_gw.so
	$(CXX) -shared -std=c++11 $(OBJ_DIR)/median.o $(BUILD_OBJ_DIR)/align.o $(BUILD_OBJ_DIR)/filters.o $(BUILD_OBJ_DIR)/plugin.o -o $(LIB_DIR)/lib_median.so
	$(CXX) -shared -std=c++11 $(OBJ_DIR)/unsharp.o $(BUILD_OBJ_DIR)/align.o $(BUILD_OBJ_DIR)/filters.o $(BUILD_OBJ_DIR)/plugin.o -o $(LIB_DIR)/lib_unsharp.so

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -f lib_autoc.so lib_gw.so lib_median.so lib_unsharp.so
	mkdir -p $(OBJ_DIR) 
	

