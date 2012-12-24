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
#include "../Game.h"

using namespace v8;
namespace Game { namespace api { namespace mouse {


    // API --------------------------------------------------------------------
    Handle<Value> hide(const Arguments& args) {
        al_hide_mouse_cursor(Game::display);
        return Undefined();
    }
    
    Handle<Value> show(const Arguments& args) {
        al_show_mouse_cursor(Game::display);
        return Undefined();
    }

    Handle<Value> grab(const Arguments& args) {
        al_grab_mouse(Game::display);
        return Undefined();
    }
    
    Handle<Value> ungrab(const Arguments& args) {
        al_ungrab_mouse();
        return Undefined();
    }

    Handle<Value> setCursor(const Arguments& args) {

        ALLEGRO_SYSTEM_MOUSE_CURSOR cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
        if (args.Length() > 0) {
            switch(ToInt32(args[0])) {
                case ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_PROGRESS:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PROGRESS;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT;
                    break;

                case ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE:
                    cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE;
                    break;
                
            }
        }

        if (al_set_system_mouse_cursor(Game::display, cursor)) {
            return True();
                
        } else {
            return False();
        }

    }

    Handle<Value> isDown(const Arguments& args) {
        
        bool ok = false;
        if (args.Length() > 0) {
            int id = ToInt32(args[0]);
            if (id > 0 && id < 3) {
                ok = Game::mouse.state[id] > 0;
            }
        }

        return ok? v8::True() : v8::False();

    }

    Handle<Value> wasPressed(const Arguments& args) {

        bool ok = false;
        if (args.Length() > 0) {
            int id = ToInt32(args[0]);
            if (id > 0 && id < 3) {
                ok = Game::mouse.state[id] == 1;
            }
        }

        return ok? v8::True() : v8::False();

    }

    Handle<Value> wasReleased(const Arguments& args) {

        bool ok = false;
        if (args.Length() > 0) {
            int id = ToInt32(args[0]);
            if (id > 0 && id < 3) {
                ok = Game::mouse.state[id] == 0 && Game::mouse.stateOld[id] > 0;
            }
        }

        return ok? v8::True() : v8::False();

    }

    Handle<Value> hasFocus(const Arguments& args) {
        return Game::mouse.hasFocus ? v8::True() : v8::False();
    }

    Handle<Value> getCount(const Arguments& args) {
        return Number::New(Game::mouse.pressedCount);
    }

    Handle<Value> getPosition(const Arguments& args) {

        int offsetX = 0;
        int offsetY = 0;
        if (args.Length() > 1) {
            offsetX = ToInt32(args[0]);
            offsetY = ToInt32(args[1]);
        }

        Handle<Object> pos = Game::templates.position->NewInstance();
        pos->Set(String::New("x"), Number::New(Game::mouse.x - Game::graphics.offsetX + offsetX));
        pos->Set(String::New("y"), Number::New(Game::mouse.y - Game::graphics.offsetY + offsetY));

        return pos;
        
    }


    // Export -----------------------------------------------------------------
    void init(Handle<Object> object) {

        object->Set(String::New("CURSOR_NONE"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_NONE));
        object->Set(String::New("CURSOR_DEFAULT"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT));
        object->Set(String::New("CURSOR_ARROW"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW));
        object->Set(String::New("CURSOR_BUSY"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY));
        object->Set(String::New("CURSOR_QUESTION"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION));
        object->Set(String::New("CURSOR_EDIT"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT));
        object->Set(String::New("CURSOR_MOVE"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE));
        object->Set(String::New("CURSOR_RESIZE_N"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N));
        object->Set(String::New("CURSOR_RESIZE_W"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W));
        object->Set(String::New("CURSOR_RESIZE_S"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S));
        object->Set(String::New("CURSOR_RESIZE_E"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E));
        object->Set(String::New("CURSOR_RESIZE_NW"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW));
        object->Set(String::New("CURSOR_RESIZE_SW"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW));
        object->Set(String::New("CURSOR_RESIZE_SE"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE));
        object->Set(String::New("CURSOR_RESIZE_NE"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE));
        object->Set(String::New("CURSOR_PROGRESS"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_PROGRESS));
        object->Set(String::New("CURSOR_PRECISION"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION));
        object->Set(String::New("CURSOR_LINK"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK));
        object->Set(String::New("CURSOR_ALT_SELECT"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT));
        object->Set(String::New("CURSOR_UNAVAILABLE"), Number::New(ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE));

        object->Set(String::New("BUTTON_LEFT"), Number::New(1));
        object->Set(String::New("BUTTON_RIGHT"), Number::New(2));

        exposeApi(object, "isDown", isDown);
        exposeApi(object, "wasPressed", wasPressed);
        exposeApi(object, "wasReleased", wasReleased);
        exposeApi(object, "hasFocus", hasFocus);
        exposeApi(object, "getCount", getCount);
        exposeApi(object, "getPosition", getCount);
        exposeApi(object, "hide", hide);
        exposeApi(object, "show", show);
        exposeApi(object, "grab", grab);
        exposeApi(object, "ungrab", ungrab);
        exposeApi(object, "setCursor", setCursor);

    }

}}}

