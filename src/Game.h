// Copyright (c) 2012 Ivo Wetzel.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#ifndef GAME_H
#define GAME_H

#include "js.h"
#include <v8.h>

#include <string>
#include <map>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_memfile.h>

#include <stdio.h>

#define MAX_MOUSE 8

#define setFunctionProp(obj, name, func) obj->Set(v8::String::NewSymbol(name), v8::FunctionTemplate::New(func)->GetFunction());
#define setNumberProp(obj, name, num) obj->Set(v8::String::NewSymbol(name), v8::Number::New(num));
#define setProp(obj, name, val) obj->Set(v8::String::NewSymbol(name), val);

#define debug(id) printf("[game::" id "]\n");
#define debugMsg(id, msg) printf("[game::" id "] " msg "\n");
#define debugArgs(id, msg, options...) printf("[game::" id "] " msg "\n", options);


// Game Namespace -------------------------------------------------------------
namespace Game {

    // Type Declarations ------------------------------------------------------
    typedef std::map<const std::string, v8::Persistent<v8::Value> > ModuleMap;

    typedef struct {
        ALLEGRO_DISPLAY *display;
        ALLEGRO_BITMAP *background;
        ALLEGRO_EVENT_QUEUE *eventQueue;
        ALLEGRO_TIMER *timer;
        ALLEGRO_MIXER *mixer;
        ALLEGRO_VOICE *voice;
        
    } Allegro;

    typedef struct {
        v8::Persistent<v8::Context> context;
        v8::Persistent<v8::Object> global;
        v8::Persistent<v8::Object> game;
        v8::Persistent<v8::Object> console;
        v8::Persistent<v8::Object> keyboard;
        v8::Persistent<v8::Object> mouse;
        v8::Persistent<v8::Object> graphics;
        v8::Persistent<v8::Object> image;
        v8::Persistent<v8::Object> music;
        v8::Persistent<v8::Object> sound;
        
    } JS;        

    typedef struct {
        bool running;
        bool paused;
        bool reload;
        std::string main;
        bool error;

    } State;

    typedef struct {
        double time;
        double delta;

    } Time;

    typedef struct {
        int x;
        int y;
        int pressedCount;
        bool hasFocus;
        int state[MAX_MOUSE];
        int stateOld[MAX_MOUSE];

    } Mouse;

    typedef struct {
        int pressedCount;
        bool hasFocus;
        int state[ALLEGRO_KEY_MAX];
        int stateOld[ALLEGRO_KEY_MAX];

    } Keyboard;

    typedef struct {
        std::string title;
        int width;
        int height;
        int scale;
        int fps;
        int lineWidth;
        int offsetX;
        int offsetY;
        ALLEGRO_COLOR color;
        ALLEGRO_COLOR bgColor;
        ALLEGRO_COLOR blendColor;

        bool wasResized;

    } Graphics;

    typedef struct {
        v8::Persistent<v8::ObjectTemplate> position;
        v8::Persistent<v8::ObjectTemplate> size;
        v8::Persistent<v8::ObjectTemplate> color;
        
    } Templates;


    // V8 ---------------------------------------------------------------------
    extern Templates templates;
    extern ModuleMap *moduleCache;

    // State
    extern Allegro allegro;
    extern JS js;
    extern State state;
    extern Time time;
    extern Mouse mouse;
    extern Keyboard keyboard;
    extern Graphics graphics;


    // Methods ----------------------------------------------------------------
    bool init(const std::string filename);
    void setup();
    void reset();
    int loop();
    void exit();

    bool initAllegro();
    bool initJS();

    bool invoke(const char *name, v8::Handle<v8::Value> *args, int argc);
    v8::Handle<v8::Value> require(const v8::Arguments& args);
    v8::Handle<v8::Value> requireModule(std::string module);


    // JavaScript API ---------------------------------------------------------
    namespace api {
            
        v8::Handle<v8::Value> require(const v8::Arguments& args);

        namespace console {
            void init(v8::Handle<v8::Object> object);
        }

        namespace game {
            void init(v8::Handle<v8::Object> object);
        }

        namespace keyboard {
            void init(v8::Handle<v8::Object> object);
        }

        namespace mouse {
            void init(v8::Handle<v8::Object> object);
        }

        namespace graphics {
            void init(v8::Handle<v8::Object> object);
        }

        namespace image {
            void init(v8::Handle<v8::Object> object);
            void shutdown();
        }

        namespace sound {
            void init(v8::Handle<v8::Object> object);
            void update(double time, double dt);
            void shutdown();
        }

        namespace music {
            void init(v8::Handle<v8::Object> object);
            void update(double time, double dt);
            void shutdown();
        }

    }

    // File IO ----------------------------------------------------------------
    namespace io {

        namespace file {
            ALLEGRO_FILE *open(std::string filename);
            bool close(ALLEGRO_FILE *fp);
        }

        namespace image {
            ALLEGRO_BITMAP *open(std::string filename);
        }

        namespace sample {
            ALLEGRO_SAMPLE *open(std::string filename);
        }

        namespace stream {
            ALLEGRO_AUDIO_STREAM *open(std::string filename);
        }

    }
    
    // End Namespace ----------------------------------------------------------

}

#endif

