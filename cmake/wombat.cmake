set(API src/io/file.cpp src/io/image.cpp src/io/sample.cpp src/io/stream.cpp)
set(IO src/api/console.cpp src/api/game.cpp src/api/keyboard.cpp src/api/mouse.cpp src/api/graphics.cpp src/api/image.cpp src/api/music.cpp src/api/sound.cpp )
set(CORE src/Game.cpp src/js.cpp)

ADD_DEFINITIONS(-g -Wall -W -Wpointer-arith -Wcast-qual -ggdb)
add_executable(wombat main.cpp ${CORE} ${API} ${IO})

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/res)
target_link_libraries(wombat v8 allegro allegro_memfile allegro_primitives allegro_image allegro_audio allegro_acodec)

