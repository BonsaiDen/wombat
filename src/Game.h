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

#define MAX_MOUSE 8
#define exposeApi(obj, name, func) obj->Set(String::New(name), FunctionTemplate::New(func)->GetFunction());

#include <string>
#include <map>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_memfile.h>
#include <v8.h>
#include "js.h"

using namespace v8;
using namespace std;

// Game Namespace -------------------------------------------------------------
namespace Game {

    // Type Declarations ------------------------------------------------------
    struct Image;
    typedef struct Image {
        ALLEGRO_BITMAP *bitmap;
        int cols;
        int rows;

    } Image;

    typedef map<const string, Image*> ImageMap;
    typedef map<const string, Handle<Value> > ModuleMap;

    // State Structs ----------------------------------------------------------
    struct JS;
    typedef struct JS {
        Handle<Object> global;
        Handle<Object> game;
        Handle<Object> console;
        Handle<Object> keyboard;
        Handle<Object> mouse;
        Handle<Object> graphics;
        Handle<Object> image;
        Handle<Object> sound;
        
    } JS;        

    struct State;
    typedef struct State {
        bool running;
        bool paused;
        bool reload;
        string main;
        bool error;
    } State;

    struct Time;
    typedef struct Time {
        double time;
        double delta;
    } Time;

    struct Mouse;
    typedef struct Mouse {
        int x;
        int y;
        int pressedCount;
        bool hasFocus;
        int state[MAX_MOUSE];
        int stateOld[MAX_MOUSE];
    } Mouse;

    struct Keyboard;
    typedef struct Keyboard {
        int pressedCount;
        bool hasFocus;
        int state[ALLEGRO_KEY_MAX];
        int stateOld[ALLEGRO_KEY_MAX];
    } Keyboard;

    struct Graphics;
    typedef struct Graphics {
        string title;
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

    struct Templates;
    typedef struct Templates {
        Handle<ObjectTemplate> position;
        Handle<ObjectTemplate> size;
        Handle<ObjectTemplate> color;
        
    } Templates;


    // V8 ---------------------------------------------------------------------
    extern struct Templates templates;
    extern ModuleMap *moduleCache;

    // General Allegro state
    extern ALLEGRO_DISPLAY *display;
    extern ALLEGRO_BITMAP *background;
    extern ALLEGRO_EVENT_QUEUE *eventQueue;
    extern ALLEGRO_TIMER *timer;

    // State
    extern struct JS js;
    extern struct State state;
    extern struct Time time;
    extern struct Mouse mouse;
    extern struct Keyboard keyboard;
    extern struct Graphics graphics;
    extern ImageMap *images;


    // Methods ----------------------------------------------------------------
    bool init(Persistent<Context> context);
    bool start(string filename);
    void reset();
    int loop();

    void initV8(Persistent<Context> context);
    void initAPI();
    bool initAllegro();
    bool initJS();

    inline void dump(const char* msg) {
        printf("%s\n", msg);
    }

    bool callGameMethod(const char *name, Handle<Value> *args, int argc);
    Handle<Value> require(const Arguments& args);
    Handle<Value> requireModule(string module);


    // JavaScript API ---------------------------------------------------------
    namespace api {
            
        Handle<Value> require(const Arguments& args);

        namespace console {
            void init(Handle<Object> object);
        }

        namespace game {
            void init(Handle<Object> object);
        }

        namespace keyboard {
            void init(Handle<Object> object);
        }

        namespace mouse {
            void init(Handle<Object> object);
        }

        namespace graphics {
            void init(Handle<Object> object);
        }

        namespace image {
            void init(Handle<Object> object);
        }

    }

    // File IO ----------------------------------------------------------------
    namespace io {

        namespace file {
            ALLEGRO_FILE *open(string filename);
        }

        namespace image {
            ALLEGRO_BITMAP *open(string filename);
        }

    }
    
    // End Namespace ----------------------------------------------------------

}

#endif

