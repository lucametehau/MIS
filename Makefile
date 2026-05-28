CXX = g++
NVCC = nvcc
CXXFLAGS = -Wall -Wextra -std=c++20 -O3 -fopenmp
NVCCFLAGS = -O3 -std=c++17 -arch=sm_80 --expt-relaxed-constexpr

# Define our directories
SRC_DIR = src
OBJ_DIR = obj

ifeq ($(OS),Windows_NT)
    TARGET = mis.exe
    
    # Windows paths
    SRC = $(wildcard $(SRC_DIR)/*.cpp)
    CU_SRC = $(wildcard $(SRC_DIR)/*.cu)
    OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC)) $(patsubst $(SRC_DIR)/%.cu,$(OBJ_DIR)/%.cu.o,$(CU_SRC))
    DEPS = $(OBJ:.o=.d)
    
    comma := ,
    space := $(subst ,, )
    FILES_TEMP := $(OBJ) $(TARGET) $(DEPS)
    FILES := $(subst $(space),$(comma) ,$(FILES_TEMP))
    
    CLEAN_CMD := powershell "rm -Force $(FILES) -ErrorAction SilentlyContinue"
    MKDIR_CMD := powershell "if (-not (Test-Path $(OBJ_DIR))) { New-Item -ItemType Directory -Force -Path $(OBJ_DIR) }"
    LDFLAGS = -lcudart
else
    TARGET = mis
    
    # Unix paths
    SRC = $(wildcard $(SRC_DIR)/*.cpp)
    CU_SRC = $(wildcard $(SRC_DIR)/*.cu)
    OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC)) $(patsubst $(SRC_DIR)/%.cu,$(OBJ_DIR)/%.cu.o,$(CU_SRC))
    DEPS = $(OBJ:.o=.d)
    
    CLEAN_CMD := rm -rf $(OBJ_DIR) $(TARGET)
    MKDIR_CMD := mkdir -p $(OBJ_DIR)
    LDFLAGS = -L/opt/cuda/lib64 -lcudart
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile .cpp to .o inside the obj/ directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR_CMD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.cu.o: $(SRC_DIR)/%.cu
	@$(MKDIR_CMD)
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

clean:
	$(CLEAN_CMD)

debug:
	$(MAKE) CXXFLAGS="-Wall -Wextra -std=c++20 -g -DDEBUG -O0" NVCCFLAGS="-g -G -O0"

test:
	$(MAKE) CXXFLAGS="-Wall -Wextra -std=c++20 -g -DDEBUG -DTEST -O0" NVCCFLAGS="-g -G -O0"

-include $(DEPS)
