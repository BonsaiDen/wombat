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

using namespace v8;

// Game Namespace -------------------------------------------------------------
namespace Game {

    // V8
    struct Templates templates;
    ModuleMap *moduleCache;

    // State
    struct Allegro allegro;
    struct JS js;
    struct State state;
    struct Time time;
    struct Mouse mouse;
    struct Keyboard keyboard;
    struct Graphics graphics;

    // Methods ----------------------------------------------------------------
    // ------------------------------------------------------------------------
    bool init(std::string filename) {

        debug("init");
        setup();

        // Use custom game file
        if (filename.length()) {

            state.main = filename;

            // split path and chdir to the new base if required
            size_t found;
            found = filename.find_last_of("/\\");
            if (found != filename.npos) {

                std::string path = filename.substr(0, found);
                state.main = filename.substr(found + 1);
                if (chdir(path.data())) {
                    return false;
                }
                
                debugArgs("init", "new base is '%s'", path.data());

            }

        }

        reset();

        if (!initJS() || !initAllegro()) {
            exit();
            return false;

        } else {
            invoke("load", NULL, 0);
            return true;
        }

    }

    void setup() {

        // Version information
        uint32_t version = al_get_allegro_version();
        debugArgs("setup::version", "Allegro %d.%d.%d.%d / v8 %s",
                    version >> 24, (version >> 16) & 255, (version >> 8) & 255,
                    version & 255, V8::GetVersion());


        // V8 and API Exposure ------------------------------------------------
        js.context = Context::New();
        Context::Scope contextScope(js.context);

        // Setup global objects
        HandleScope scope;

        js.global = Persistent<Object>::New(js.context->Global());
        js.game = JSObject();
        js.console = JSObject();
        js.keyboard = JSObject();
        js.mouse = JSObject();
        js.graphics = JSObject();
        js.image = JSObject();
        js.music = JSObject();
        js.sound = JSObject();

        // Initiate Object Templates
        templates.position = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
        setNumberProp(templates.position, "x", 0);
        setNumberProp(templates.position, "y", 0);

        templates.size = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
        setNumberProp(templates.size, "w", 0);
        setNumberProp(templates.size, "h", 0);

        templates.color = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
        setNumberProp(templates.color, "r", 1);
        setNumberProp(templates.color, "g", 1);
        setNumberProp(templates.color, "b", 1);
        setNumberProp(templates.color, "a", 1);

        // Initiate Script Cache for require
        api::game::init(js.game);
        api::console::init(js.console);
        api::keyboard::init(js.keyboard);
        api::mouse::init(js.mouse);
        api::graphics::init(js.graphics);
        api::image::init(js.image);
        api::music::init(js.music);
        api::sound::init(js.sound);

        // Expose mapped API to JavaScript
        setProp(js.global, "console", js.console);
        setProp(js.global, "game", js.game);
        setProp(js.global, "keyboard", js.keyboard);
        setProp(js.global, "mouse", js.mouse);
        setProp(js.global, "graphics", js.graphics);
        setProp(js.global, "image", js.image);
        setProp(js.global, "music", js.music);
        setProp(js.global, "sound", js.sound);
        setFunctionProp(js.global, "require", require);


        // Game State Specific ------------------------------------------------

        // Allegro Things
        allegro.eventQueue = NULL;
        allegro.timer = NULL;
        allegro.display = NULL;
        allegro.background = NULL;

        // State and Time
        state.running = false;
        state.paused = false;
        state.reload = false;
        state.error = false;
        state.main = "game";

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

        // Resources
        moduleCache = new ModuleMap();

    }

    void reset() {

        debug("reset");

        Context::Scope contextScope(js.context);
        HandleScope scope;

        for(ModuleMap::iterator it = moduleCache->begin(); it != moduleCache->end(); it++) {
            it->second.Dispose();
            it->second.Clear();
        }
        moduleCache->clear();

        state.error = false;
        requireModule(state.main);
        
    }

    int loop() {

        debug("loop");
        
        Context::Scope contextScope(js.context);
        HandleScope scope;
        Handle<Value> args[2];

        double lastFrameTime = 0, now = 0;
        bool redraw = false;
        unsigned int i = 0;

        al_start_timer(allegro.timer);
        state.running = true;
        while (state.running) {

            ALLEGRO_EVENT event;
            al_wait_for_event(allegro.eventQueue, &event);

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

                    // Check pending sample instances
                    api::sound::update(now, time.delta);
                    api::music::update(now, time.delta);

                    // Call Game Update Code
                    args[0] = Number::New(time.time);
                    args[1] = Number::New(time.delta);
                    invoke("update", args, 2);

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

            // Render 
            if (redraw && al_is_event_queue_empty(allegro.eventQueue)) {

                // Handle resizing
                if (graphics.wasResized) {

                    al_resize_display(allegro.display, graphics.width * graphics.scale, graphics.height * graphics.scale);

                    if (allegro.background != NULL) {
                        al_destroy_bitmap(allegro.background);
                    }

                    if (graphics.scale != 1) {
                        allegro.background = al_create_bitmap(graphics.width, graphics.height);
                        al_set_target_bitmap(allegro.background);

                    } else {
                        al_set_target_bitmap(al_get_backbuffer(allegro.display));
                    }

                    graphics.wasResized = false;

                }

                if (graphics.scale != 1) {
                    al_set_target_bitmap(allegro.background);
                }

                al_clear_to_color(graphics.bgColor);

                // Call Game Render Code
                args[0] = Number::New(time.time);
                invoke("render", args, 1);

                // Scale up if necessary
                if (graphics.scale != 1) {
                    al_set_target_bitmap(al_get_backbuffer(allegro.display));
                    al_draw_scaled_bitmap(allegro.background, 0, 0, graphics.width, graphics.height, 0, 0, 
                                          graphics.width * graphics.scale, graphics.height * graphics.scale, 0);
                }

                al_flip_display();
                redraw = false;

            }

        }

        exit();
        return 0;
 
    }

    void exit() {

        // Clean up allegro ---------------------------------------------------
        debugMsg("exit", "Destroy Allegro");
        if (allegro.mixer) {
            al_destroy_mixer(allegro.mixer);
        }

        if (allegro.voice) {
            al_destroy_voice(allegro.voice);
        }

        al_destroy_timer(allegro.timer);
        al_destroy_event_queue(allegro.eventQueue);
        al_destroy_display(allegro.display);

        if (allegro.background != NULL) {
            al_destroy_bitmap(allegro.background);
        }
        
        // Remove cached modules
        debugMsg("exit", "Destroy Modules");
        for(ModuleMap::iterator it = moduleCache->begin(); it != moduleCache->end(); it++) {
            it->second.Dispose();
            it->second.Clear();
        }
        moduleCache->clear();

        debugMsg("exit", "Shutdown APIs");
        api::image::shutdown();
        api::music::shutdown();
        api::sound::shutdown();

        // Cleanup APIs
        debugMsg("exit", "Destroy JS");
        js.game.Dispose();
        js.console.Dispose();
        js.keyboard.Dispose();
        js.mouse.Dispose();
        js.graphics.Dispose();
        js.image.Dispose();
        js.music.Dispose();
        js.sound.Dispose();
        
        // Remove templates
        templates.position.Dispose();
        templates.size.Dispose();
        templates.color.Dispose();
        
        // Remove context
        js.global.Dispose();
        js.context.Dispose();
        js.context.Clear();

    }


    // Initiliazation Methods -------------------------------------------------
    bool initJS() {
        
        Context::Scope contextScope(js.context);
        HandleScope scope;
            
        // Setup config object passed to JS
        Handle<Object> config = JSObject();
        config->Set(String::New("title"), String::New(graphics.title.data()));
        config->Set(String::New("width"), Number::New(graphics.width));
        config->Set(String::New("height"), Number::New(graphics.height));
        config->Set(String::New("scale"), Number::New(graphics.scale));
        config->Set(String::New("fps"), Number::New(graphics.fps));

        Handle<Value> args[1];
        args[0] = config;
        invoke("init", args, 1);

        graphics.width = ToInt32(config->Get(String::New("width")));
        graphics.height = ToInt32(config->Get(String::New("height")));
        graphics.scale = ToInt32(config->Get(String::New("scale")));
        graphics.fps = ToInt32(config->Get(String::New("fps")));

        String::Utf8Value text(config->Get(String::New("title")));
        graphics.title.clear();
        graphics.title.append(*text);

        if (graphics.width * graphics.scale <= 0 || graphics.width >= 1024 * graphics.scale) {
            debugMsg("initJS", "Invalid graphics.width");
            return false;

        } else if (graphics.height * graphics.scale <= 0 || graphics.height >= 768 * graphics.scale) {
            debugMsg("initJS", "Invalid graphics.height");
            return false;

        } else if (graphics.fps <= 0 || graphics.fps > 60) {
            debugMsg("initJS", "Invalid graphics.fps");
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
            debugMsg("initAllegro", "al_init() failed");
            return false;
        }

        if (!al_init_image_addon() || !al_init_primitives_addon()) {
            debugMsg("initAllegro", "al_init_image_addon() failed");
            return false;
        }

        allegro.timer = al_create_timer(1.0 / graphics.fps);
        allegro.eventQueue = al_create_event_queue();
        al_install_keyboard();
        al_install_mouse();

        keyboard.hasFocus = true;

        al_register_event_source(allegro.eventQueue, al_get_keyboard_event_source());
        al_register_event_source(allegro.eventQueue, al_get_mouse_event_source());
        al_register_event_source(allegro.eventQueue, al_get_timer_event_source(allegro.timer));


        // Setup Display ------------------------------------------------------
        allegro.display = al_create_display(graphics.width * graphics.scale, graphics.height * graphics.scale);
        if (allegro.display == NULL) {
            debugMsg("initAllegro", "al_create_display() failed");
            return false;
        }

        al_set_new_display_option(ALLEGRO_VSYNC, true, ALLEGRO_SUGGEST);
        al_set_window_title(allegro.display, graphics.title.data());
        al_register_event_source(allegro.eventQueue, al_get_display_event_source(allegro.display));


        // Setup Graphics -----------------------------------------------------
        if (graphics.scale != 1) {
            allegro.background = al_create_bitmap(graphics.width, graphics.height);
            al_set_target_bitmap(allegro.background);

        } else {
            al_set_target_bitmap(al_get_backbuffer(allegro.display));
        }

        graphics.color = al_map_rgba(255, 255, 255, 255); 
        graphics.bgColor = al_map_rgba(0, 0, 0, 255); 
        graphics.blendColor = al_map_rgba(255, 255, 255, 255); 

        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);


        // Setup Audio --------------------------------------------------------
        if (!al_install_audio() || !al_init_acodec_addon()) {
            debugMsg("initAllegro", "al_init_audio() failed");
            return false;
        }

        allegro.voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
        allegro.mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);

        al_attach_mixer_to_voice(allegro.mixer, allegro.voice);
        if (!al_set_default_mixer(allegro.mixer)) {
            debugMsg("initAllegro", "al_set_default_mixer() failed");
            return false;
        }

        return true;
 
    }


    // V8 Helpers -------------------------------------------------------------
    // ------------------------------------------------------------------------
    bool invoke(const char *name, Handle<Value> *args, int argc) {

        if (state.error) {
            return false;
        }

        // This function calls functions like game.update() and others on the JS side
        Context::Scope contextScope(js.context);
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

    Handle<Value> requireModule(std::string name) {

        Handle<Value> exports;

        ModuleMap::iterator it = moduleCache->find(name);
        if (it == moduleCache->end()) {
            debugArgs("module::disk", "'%s' required", name.data());

            Handle<Object> module = Handle<Object>::Cast(executeScript(loadScript(name.data())));
            exports = module->Get(String::New("exports"));
            moduleCache->insert(std::make_pair(name, Persistent<Value>::New(exports)));

        } else {
            exports = it->second;
        }

        return exports;

    }

}

