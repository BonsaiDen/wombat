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
#include "Game.h"

using namespace std;

// Game Namespace -------------------------------------------------------------
namespace Game {

    // V8
    struct Templates templates;
    ModuleMap *moduleCache;

    // General Allegro state
    ALLEGRO_DISPLAY *display;
    ALLEGRO_BITMAP *background;
    ALLEGRO_EVENT_QUEUE *eventQueue;
    ALLEGRO_TIMER *timer;

    // State
    struct JS js;
    struct State state;
    struct Time time;
    struct Mouse mouse;
    struct Keyboard keyboard;
    struct Graphics graphics;
    ImageMap *images;


    bool init(Persistent<Context> context) {

        initV8(context);
        initAPI();

        // Allegro Things
        eventQueue = NULL;
        timer = NULL;
        display = NULL;
        background = NULL;

        // State and Time
        state.running = false;
        state.paused = false;
        state.reload = false;
        state.main = "game";
        state.error = false;

        time.time = 0;
        time.delta = 0;

        // Input
        mouse.x = 0;
        mouse.y = 0;
        mouse.hasFocus = false;
        mouse.pressedCount = 0;

        keyboard.hasFocus = false;
        keyboard.pressedCount = 0;

        // Graphics
        graphics.title = "Game";
        graphics.width = 640;
        graphics.height = 480;
        graphics.scale = 1;
        graphics.fps = 60;

        graphics.wasResized = false;
        graphics.lineWidth = 1;
        graphics.offsetX = 0;
        graphics.offsetY = 0;

        // Images
        images = new ImageMap();

        // TODO add error checking
        return true;

    }

    bool start(string filename) {

        // Use custom game file
        if (filename.length()) {

            state.main = filename;

            // split path and chdir to the new base if required
            size_t found;
            found = filename.find_last_of("/\\");
            if (found != filename.npos) {

                string path = filename.substr(0, found);
                state.main = filename.substr(found + 1);

                printf("[game::start] new base is '%s'\n", path.data());

                if (chdir(path.data())) {
                    return false;
                }
                
            }

        }

        reset();

        if (!initJS()) {
            return false;

        } else if (!initAllegro()) {
            return false;

        } else {
            callGameMethod("load", NULL, 0);
            return true;
        }

    }

    void reset() {

        //Handle<Array> keys = js.global->GetPropertyNames();
        //for (unsigned int i = 0; i < keys->Length(); i++) {

            //Handle<String> name = keys->Get(Number::New(i))->ToString();
            //js.global->ForceDelete(name);
            //String::Utf8Value text(name);
            //printf("%s\n", *text);

        //}

        // Expose mapped API to JavaScript
        js.global->Set(String::New("console"), js.console);
        js.global->Set(String::New("game"), js.game);
        js.global->Set(String::New("keyboard"), js.keyboard);
        js.global->Set(String::New("mouse"), js.mouse);
        js.global->Set(String::New("graphics"), js.graphics);
        js.global->Set(String::New("image"), js.image);
        js.global->Set(String::New("sound"), js.sound);

        moduleCache->clear();
        state.error = false;
        exposeApi(js.global, "require", require);
        requireModule(state.main);
        
    }

    int loop() {
        
        double lastFrameTime = 0, now = 0;
        bool redraw = true;
        unsigned int i = 0;
        
        HandleScope scope;
        Handle<Value> args[2];

        al_start_timer(timer);

        state.running = true;
        while (state.running) {

            ALLEGRO_EVENT event;
            al_wait_for_event(eventQueue, &event);

            // Handle Events
            switch (event.type) {
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    state.running = false;
                    break;

                case ALLEGRO_EVENT_KEY_DOWN:
                    keyboard.state[event.keyboard.keycode] = 1;
                    keyboard.pressedCount++;
                    break;
                
                case ALLEGRO_EVENT_KEY_UP:

                    keyboard.state[event.keyboard.keycode] = 0;
                    keyboard.pressedCount--;
                    if (keyboard.pressedCount < 0) {
                        keyboard.pressedCount = 0;
                    }

                    break;
                
                case ALLEGRO_EVENT_MOUSE_AXES:
                    mouse.x = event.mouse.x / graphics.scale;
                    mouse.y = event.mouse.y / graphics.scale;
                    break;
                
                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                    for(i = 0; i < 4; i++) {
                        if (event.mouse.button & (1 << i)) {
                            mouse.state[i + 1] = 1;
                            mouse.pressedCount++;
                        }
                    }
                    break;
                
                case ALLEGRO_EVENT_MOUSE_BUTTON_UP:

                    for(i = 0; i < 4; i++) {
                        if (event.mouse.button & (1 << i)) {
                            mouse.state[i + 1] = 0;
                            mouse.pressedCount--;
                        }
                    }

                    if (mouse.pressedCount < 0) {
                        mouse.pressedCount = 0;
                    }

                    break;
                
                case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                    mouse.hasFocus = true;
                    break;

                case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
                    mouse.hasFocus = false;
                    break;

                case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
                    keyboard.hasFocus = true;
                    break;

                case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
                    keyboard.hasFocus = false;
                    break;

                case ALLEGRO_EVENT_TIMER:

                    // Timer
                    now = al_get_time();
                    time.delta = now - lastFrameTime;
                    if (state.paused) {
                        time.delta = 0;
                    }

                    time.time += time.delta;
                    lastFrameTime = now;

                    // Call Game Update Code
                    args[0] = Number::New(time.time);
                    args[1] = Number::New(time.delta);
                    callGameMethod("update", args, 2);

                    // Update / Reset Input States
                    for(i = 0; i < ALLEGRO_KEY_MAX; i++) {

                        if (keyboard.state[i] == 1) {
                            keyboard.state[i] = 2;
                        }

                        keyboard.stateOld[i] = keyboard.state[i];

                    }

                    for(i = 0; i < MAX_MOUSE; i++) {

                        if (mouse.state[i] == 1) {
                            mouse.state[i] = 2;
                        }

                        mouse.stateOld[i] = mouse.state[i];

                    }   

                    redraw = true;
                    
                    // Handle hot code reloading
                    if (state.reload) {
                        redraw = false;
                        state.reload = false;
                        reset();
                    }
                    break;

                default:
                    break;

            }


            // Render it out
            if (redraw && al_is_event_queue_empty(eventQueue)) {

                // Handle resizing
                if (graphics.wasResized) {

                    al_resize_display(display, graphics.width * graphics.scale, graphics.height * graphics.scale);

                    if (background != NULL) {
                        al_destroy_bitmap(background);
                    }

                    if (graphics.scale != 1) {
                        background = al_create_bitmap(graphics.width, graphics.height);
                        al_set_target_bitmap(background);

                    } else {
                        al_set_target_bitmap(al_get_backbuffer(display));
                    }

                    graphics.wasResized = false;

                }

                if (graphics.scale != 1) {
                    al_set_target_bitmap(background);
                }

                unsigned char r, g, b, a;
                al_unmap_rgba(graphics.bgColor, &r, &g, &b, &a);

                al_clear_to_color(graphics.bgColor);

                // Call Game Render Code
                args[0] = Number::New(time.time);
                callGameMethod("render", args, 1);

                // Scale up if necessary
                if (graphics.scale != 1) {
                    al_set_target_bitmap(al_get_backbuffer(display));
                    al_draw_scaled_bitmap(background, 0, 0, graphics.width, graphics.height, 0, 0, 
                                          graphics.width * graphics.scale, graphics.height * graphics.scale, 0);
                }

                al_flip_display();
                redraw = false;

            }

        }

        return 0;
 
    }


    // Initiliazation Methods -------------------------------------------------
    void initV8(Persistent<Context> context) {
        
        // Setup API Exposure
        moduleCache = new ModuleMap();

        js.global = context->Global();
        js.game = JSObject();
        js.console = JSObject();
        js.keyboard = JSObject();
        js.mouse = JSObject();
        js.graphics = JSObject();
        js.image = JSObject();
        js.sound = JSObject();

        // Initiate Object Templates
        templates.position = ObjectTemplate::New();
        templates.position->Set(String::New("x"), Number::New(0));
        templates.position->Set(String::New("y"), Number::New(0));

        templates.size = ObjectTemplate::New();
        templates.size->Set(String::New("w"), Number::New(0));
        templates.size->Set(String::New("h"), Number::New(0));

        templates.color = ObjectTemplate::New();
        templates.color->Set(String::New("r"), Number::New(1));
        templates.color->Set(String::New("g"), Number::New(1));
        templates.color->Set(String::New("b"), Number::New(1));
        templates.color->Set(String::New("a"), Number::New(1));

    }

    void initAPI() {
        api::game::init(js.game);
        api::console::init(js.console);
        api::keyboard::init(js.keyboard);
        api::mouse::init(js.mouse);
        api::graphics::init(js.graphics);
        api::image::init(js.image);
    }

    bool initJS() {
            
        // Setup config object passed to JS
        Handle<Object> config = JSObject();
        config->Set(String::New("title"), String::New(graphics.title.data()));
        config->Set(String::New("width"), Number::New(graphics.width));
        config->Set(String::New("height"), Number::New(graphics.height));
        config->Set(String::New("scale"), Number::New(graphics.scale));
        config->Set(String::New("fps"), Number::New(graphics.fps));

        Handle<Value> args[1];
        args[0] = config;
        callGameMethod("init", args, 1);

        graphics.width = ToInt32(config->Get(String::New("width")));
        graphics.height = ToInt32(config->Get(String::New("height")));
        graphics.scale = ToInt32(config->Get(String::New("scale")));
        graphics.fps = ToInt32(config->Get(String::New("fps")));

        String::Utf8Value text(config->Get(String::New("title")));
        graphics.title.clear();
        graphics.title.append(*text);

        if (graphics.width * graphics.scale <= 0 || graphics.width >= 1024 * graphics.scale) {
            dump("config: Invalid graphics.width");
            return false;

        } else if (graphics.height * graphics.scale <= 0 || graphics.height >= 768 * graphics.scale) {
            dump("config: Invalid graphics.height");
            return false;

        } else if (graphics.fps <= 0 || graphics.fps > 60) {
            dump("config: Invalid frame rate");
            return false;
        }

        return true;
 
    }

    bool initAllegro() {
            
        // Setup input state
        unsigned int i;
        for(i = 0; i < ALLEGRO_KEY_MAX; i++) keyboard.state[i] = 0;
        for(i = 0; i < MAX_MOUSE; i++) mouse.state[i] = 0;

        // Init Allegro
        if (!al_init()) {
            dump("init: Failed to initialize allegro::core");
            return false;
        }

        if (!al_init_image_addon() || !al_init_primitives_addon()) {
            dump("init: Failed to initialize allegro::addons");
            return false;
        }

        // Setup Display
        display = al_create_display(graphics.width * graphics.scale, graphics.height * graphics.scale);
        if (display == NULL) {
            dump("init: Failed to create allegro::display");
            return false;
        }

        al_set_new_display_option(ALLEGRO_VSYNC, true, ALLEGRO_SUGGEST);
        al_set_window_title(display, graphics.title.data());
        keyboard.hasFocus = true;

        // Setup Timer
        timer = al_create_timer(1.0 / graphics.fps);

        // Setup events
        eventQueue = al_create_event_queue();
        al_install_keyboard();
        al_install_mouse();

        al_register_event_source(eventQueue, al_get_keyboard_event_source());
        al_register_event_source(eventQueue, al_get_mouse_event_source());
        al_register_event_source(eventQueue, al_get_display_event_source(display));
        al_register_event_source(eventQueue, al_get_timer_event_source(timer));


        // Setup Graphics
        if (graphics.scale != 1) {
            background = al_create_bitmap(graphics.width, graphics.height);
            al_set_target_bitmap(background);

        } else {
            al_set_target_bitmap(al_get_backbuffer(display));
        }

        graphics.color = al_map_rgba(255, 255, 255, 255); 
        graphics.bgColor = al_map_rgba(0, 0, 0, 255); 
        graphics.blendColor = al_map_rgba(255, 255, 255, 255); 

        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

        return true;
 
    }


    // JavaScript API ---------------------------------------------------------
    bool callGameMethod(const char *name, Handle<Value> *args, int argc) {

        if (state.error) {
            return false;
        }

        // This function calls functions like game.update() and others on the JS side
        HandleScope scope;
        Handle<Value> object = js.game->Get(String::NewSymbol(name));
        
        if (object->IsFunction()) {

            Handle<Function> func = Handle<Function>::Cast(object);
            TryCatch t = TryCatch();
            func->Call(js.global, argc, args);

            if (t.HasCaught()) {
                handleException(t);
                state.error = true;
                return false;

            } else {
                return true;
            }
        
        } else {
            return false;
        }

    }

    Handle<Value> require(const Arguments& args) {
        if (args.Length() >= 0) {
            return Game::requireModule(ToString(args[0]));

        } else {
            return Undefined();
        }
    }

    Handle<Value> requireModule(string name) {

        printf("Require module %s...\n", name.data());

        Handle<Value> exports;

        ModuleMap::iterator it = moduleCache->find(name);
        if (it == moduleCache->end()) {
            printf("Require module %s from disk\n", name.data());
            
            exports = executeScript(loadScript(name.data()));
            moduleCache->insert(std::make_pair(name, exports));

        } else {
            printf("Require module %s from cached\n", name.data());
            exports = it->second;
        }

        return exports;

    }

}

