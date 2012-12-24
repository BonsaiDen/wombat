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
namespace Game { namespace api { namespace game {

    // API --------------------------------------------------------------------
    Handle<Value> getTime(const Arguments& args) {
        return Number::New(Game::time.time);
    }

    Handle<Value> getDelta(const Arguments& args) {
        return Number::New(Game::time.delta);
    }

    Handle<Value> resume(const Arguments& args) {
        if (Game::state.paused) {
            Game::state.paused = false;
            return True();

        } else {
            return False();
        }
    }

    Handle<Value> pause(const Arguments& args) {
        if (Game::state.paused) {
            return False();

        } else {
            Game::state.paused = true;
            return True();
        }
    }

    Handle<Value> isPaused(const Arguments& args) {
        return Boolean::New(Game::state.paused);
    }

    Handle<Value> reload(const Arguments& args) {
        Game::state.reload = true;
        return Undefined();
    }

    Handle<Value> quit(const Arguments& args) {
        if (Game::state.running) {
            Game::state.running = false;
            return True();

        } else {
            return False();
        }
    }

    // Export -----------------------------------------------------------------
    void init(Handle<Object> object) {

        exposeApi(object, "getTime", getTime);
        exposeApi(object, "getDelta", getDelta);
        exposeApi(object, "pause", pause);
        exposeApi(object, "resume", resume);
        exposeApi(object, "isPaused", isPaused);
        exposeApi(object, "reload", reload); 
        exposeApi(object, "quit", quit); 

    }

}}}

