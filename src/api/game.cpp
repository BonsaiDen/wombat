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

namespace Game { namespace api { namespace game {

    // API --------------------------------------------------------------------
    v8::Handle<v8::Value> getTime(const v8::Arguments& args) {
        return v8::Number::New(time.time);
    }

    v8::Handle<v8::Value> getDelta(const v8::Arguments& args) {
        return v8::Number::New(time.delta);
    }

    v8::Handle<v8::Value> resume(const v8::Arguments& args) {
        if (state.paused) {
            state.paused = false;
            return v8::True();

        } else {
            return v8::False();
        }
    }

    v8::Handle<v8::Value> pause(const v8::Arguments& args) {
        if (state.paused) {
            return v8::False();

        } else {
            state.paused = true;
            return v8::True();
        }
    }

    v8::Handle<v8::Value> isPaused(const v8::Arguments& args) {
        return v8::Boolean::New(state.paused);
    }

    v8::Handle<v8::Value> reload(const v8::Arguments& args) {
        state.reload = true;
        return v8::Undefined();
    }

    v8::Handle<v8::Value> quit(const v8::Arguments& args) {
        if (state.running) {
            state.running = false;
            return v8::True();

        } else {
            return v8::False();
        }
    }

    // Export -----------------------------------------------------------------
    void init(v8::Handle<v8::Object> object) {

        setFunctionProp(object, "getTime", getTime);
        setFunctionProp(object, "getDelta", getDelta);
        setFunctionProp(object, "pause", pause);
        setFunctionProp(object, "resume", resume);
        setFunctionProp(object, "isPaused", isPaused);
        setFunctionProp(object, "reload", reload); 
        setFunctionProp(object, "quit", quit); 

    }

}}}

