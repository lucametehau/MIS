CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++20 -O3

# Define our directories
SRC_DIR = src
OBJ_DIR = obj

ifeq ($(OS),Windows_NT)
    TARGET = mis.exe
    
    # Windows paths
    SRC = $(wildcard $(SRC_DIR)/*.cpp)
    OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
    DEPS = $(OBJ:.o=.d)
    
    comma := ,
    space := $(subst ,, )
    FILES_TEMP := $(OBJ) $(TARGET) $(DEPS)
    FILES := $(subst $(space),$(comma) ,$(FILES_TEMP))
    
    CLEAN_CMD := powershell "rm -Force $(FILES) -ErrorAction SilentlyContinue"
    MKDIR_CMD := powershell "if (-not (Test-Path $(OBJ_DIR))) { New-Item -ItemType Directory -Force -Path $(OBJ_DIR) }"
else
    TARGET = mis
    
    # Unix paths
    SRC = $(wildcard $(SRC_DIR)/*.cpp)
    OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
    DEPS = $(OBJ:.o=.d)
    
    CLEAN_CMD := rm -rf $(OBJ_DIR) $(TARGET)
    MKDIR_CMD := mkdir -p $(OBJ_DIR)
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile .cpp to .o inside the obj/ directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR_CMD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	$(CLEAN_CMD)

debug:
	$(MAKE) CXXFLAGS="-Wall -Wextra -std=c++20 -g -DDEBUG -O0"

test:
	$(MAKE) CXXFLAGS="-Wall -Wextra -std=c++20 -g -DDEBUG -DTEST -O0"

-include $(DEPS)