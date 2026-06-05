CXX = g++
NVCC = nvcc
CXXFLAGS = -Wall -Wextra -std=c++20 -O3 -fopenmp -Ithird_party
NVCCFLAGS = -O3 -lineinfo -std=c++17 -arch=sm_80 --expt-relaxed-constexpr

# Define our directories
SRC_DIR = src
OBJ_DIR = obj

# Set NO_CUDA=1 to build without GPU support (e.g. when nvcc is unavailable)
ifeq ($(NO_CUDA),1)
    CU_SRC :=
    EXTRA_SRC = $(SRC_DIR)/luby_gpu_stub.cpp
    LDFLAGS :=
else
    CU_SRC = $(wildcard $(SRC_DIR)/*.cu)
    EXTRA_SRC :=
endif

CPP_SRC = $(filter-out $(SRC_DIR)/luby_gpu_stub.cpp,$(wildcard $(SRC_DIR)/*.cpp))

ifeq ($(OS),Windows_NT)
    TARGET = mis.exe
    SRC = $(CPP_SRC) $(EXTRA_SRC)
    OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC)) $(patsubst $(SRC_DIR)/%.cu,$(OBJ_DIR)/%.cu.o,$(CU_SRC))
    DEPS = $(OBJ:.o=.d)
    comma := ,
    space := $(subst ,, )
    FILES_TEMP := $(OBJ) $(TARGET) $(DEPS)
    FILES := $(subst $(space),$(comma) ,$(FILES_TEMP))
    CLEAN_CMD := powershell "rm -Force $(FILES) -ErrorAction SilentlyContinue"
    MKDIR_CMD := powershell "if (-not (Test-Path $(OBJ_DIR))) { New-Item -ItemType Directory -Force -Path $(OBJ_DIR) }"
    ifneq ($(NO_CUDA),1)
        LDFLAGS = -lcudart
    endif
else
    TARGET = mis
    SRC = $(CPP_SRC) $(EXTRA_SRC)
    OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC)) $(patsubst $(SRC_DIR)/%.cu,$(OBJ_DIR)/%.cu.o,$(CU_SRC))
    DEPS = $(OBJ:.o=.d)
    CLEAN_CMD := rm -rf $(OBJ_DIR) $(TARGET)
    MKDIR_CMD := mkdir -p $(OBJ_DIR)
    ifneq ($(NO_CUDA),1)
        LDFLAGS = -L/opt/cuda/lib64 -lcudart
    endif
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

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
