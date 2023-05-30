SRC_DIR := src/Server
COMMON_DIR := src/SDLUtils
UTILS_DIR := src/GameUtils
NET_DIR := src/Net

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp) $(wildcard $(SRC_DIR)/*/*/*.cpp) $(wildcard $(UTILS_DIR)/*.cpp) $(wildcard $(NET_DIR)/*.cpp)
COMMON_FILES := $(wildcard $(COMMON_DIR)/*.cpp)
# NET_FILES := $(wildcard $(NET_DIR)/*.cpp)

OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(SRC_DIR)/%.o,$(SRC_FILES)) $(patsubst $(COMMON_DIR)/%.cpp,$(COMMON_DIR)/%.o,$(COMMON_FILES)) $(patsubst $(UTILS_DIR)/%.cpp,$(UTILS_DIR)/%.o,$(UTILS_FILE)) $(patsubst $(NET_DIR)/%.cpp,$(NET_DIR)/%.o,$(NET_FILES))


EXEC_FILE := TronServer.out

LDFLAGS := -L/usr/lib/x86_64-linux-gnu -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2 -lpthread
CPPFLAGS := --std=c++17 -I/usr/include/SDL2

$(EXEC_FILE): $(OBJ_FILES)
	g++ -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

clean:
	find $(SRC_DIR) -name "*.o" -type f -delete
	find $(COMMON_DIR) -name "*.o" -type f -delete
	find $(NET_DIR) -name "*.o" -type f -delete