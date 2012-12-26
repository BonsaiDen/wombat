FILES:=main.cpp src/Game.cpp src/js.cpp src/api/console.cpp src/api/game.cpp src/api/keyboard.cpp src/api/mouse.cpp src/api/graphics.cpp src/api/image.cpp src/api/music.cpp src/api/sound.cpp src/io/file.cpp src/io/image.cpp src/io/sample.cpp src/io/stream.cpp

#ALLEGRO_STATIC:=`pkg-config --libs --static allegro-static-5.0 allegro_memfile-static-5.0 allegro_primitives-static-5.0 allegro_image-static-5.0 allegro_audio-static-5.0 allegro_acodec-static-5.0` 

ALLEGRO:=-lallegro -lallegro_memfile -lallegro_primitives -lallegro_image -lallegro_audio -lallegro_acodec 
CFLAGS=-o wombat -Wall -Wextra -Wno-unused

build: $(FILES)
	g++ $(CFLAGS) $(FILES) $(ALLEGRO) -lv8

debug: $(FILES)
	g++ $(CFLAGS) $(FILES) $(ALLEGRO) -lv8 -ggdb

static: $(FILES)
	g++ $(CFLAGS) $(FILES) $(ALLEGRO_STATIC)

