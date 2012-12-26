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
        al_hide_mouse_cursor(Game::allegro.display);
        return Undefined();
    }
    
    Handle<Value> show(const Arguments& args) {
        al_show_mouse_cursor(Game::allegro.display);
        return Undefined();
    }

    Handle<Value> grab(const Arguments& args) {
        al_grab_mouse(Game::allegro.display);
        return Undefined();
    }
    
    Handle<Value> ungrab(const Arguments& args) {
        al_ungrab_mouse();
        return Undefined();
    }

    Handle<Value> setCursor(const Arguments& args) {

        ALLEGRO_SYSTEM_MOUSE_CURSOR cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
        if (args.Length() > 0) {
            cursor = (ALLEGRO_SYSTEM_MOUSE_CURSOR)ToInt32(args[0]);
        }

        if (al_set_system_mouse_cursor(Game::allegro.display, cursor)) {
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
        setNumberProp(pos, "x", Game::mouse.x - Game::graphics.offsetX + offsetX);
        setNumberProp(pos, "y", Game::mouse.y - Game::graphics.offsetY + offsetY);

        return pos;
        
    }


    // Export -----------------------------------------------------------------
    void init(Handle<Object> object) {

        setNumberProp(object, "CURSOR_NONE", ALLEGRO_SYSTEM_MOUSE_CURSOR_NONE);
        setNumberProp(object, "CURSOR_DEFAULT", ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
        setNumberProp(object, "CURSOR_ARROW", ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
        setNumberProp(object, "CURSOR_BUSY", ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);
        setNumberProp(object, "CURSOR_QUESTION", ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
        setNumberProp(object, "CURSOR_EDIT", ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT);
        setNumberProp(object, "CURSOR_MOVE", ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
        setNumberProp(object, "CURSOR_RESIZE_N", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N);
        setNumberProp(object, "CURSOR_RESIZE_W", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W);
        setNumberProp(object, "CURSOR_RESIZE_S", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S);
        setNumberProp(object, "CURSOR_RESIZE_E", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E);
        setNumberProp(object, "CURSOR_RESIZE_NW", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW);
        setNumberProp(object, "CURSOR_RESIZE_SW", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW);
        setNumberProp(object, "CURSOR_RESIZE_SE", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE);
        setNumberProp(object, "CURSOR_RESIZE_NE", ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE);
        setNumberProp(object, "CURSOR_PROGRESS", ALLEGRO_SYSTEM_MOUSE_CURSOR_PROGRESS);
        setNumberProp(object, "CURSOR_PRECISION", ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION);
        setNumberProp(object, "CURSOR_LINK", ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        setNumberProp(object, "CURSOR_ALT_SELECT", ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT);
        setNumberProp(object, "CURSOR_UNAVAILABLE", ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);

        setNumberProp(object, "BUTTON_LEFT", 1);
        setNumberProp(object, "BUTTON_RIGHT", 2);

        setFunctionProp(object, "isDown", isDown);
        setFunctionProp(object, "wasPressed", wasPressed);
        setFunctionProp(object, "wasReleased", wasReleased);
        setFunctionProp(object, "hasFocus", hasFocus);
        setFunctionProp(object, "getCount", getCount);
        setFunctionProp(object, "getPosition", getCount);
        setFunctionProp(object, "hide", hide);
        setFunctionProp(object, "show", show);
        setFunctionProp(object, "grab", grab);
        setFunctionProp(object, "ungrab", ungrab);
        setFunctionProp(object, "setCursor", setCursor);

    }

}}}

