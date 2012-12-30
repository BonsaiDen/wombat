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
#ifndef JS_H
#define JS_H

#include <v8.h>
#include <string>

v8::Handle<v8::Script> loadScript(const char *name);
v8::Handle<v8::Value> requireScript(const v8::Arguments& args);
v8::Handle<v8::Value> executeScript(const v8::Handle<v8::Script> &script);
v8::Persistent<v8::Object> JSObject();

void handleException(const v8::TryCatch &tryCatch);

inline int ToInt32(const v8::Handle<v8::Value> &i) {
    return i->Int32Value();
}

inline float ToFloat(const v8::Handle<v8::Value> &f) {
    return static_cast<float>(f->NumberValue());
}

inline float ToBoolean(const v8::Handle<v8::Value> &f) {
    return f->BooleanValue();
}

inline std::string ToString(const v8::Handle<v8::Value> &f) {
    v8::String::Utf8Value raw(f->ToString());
    return std::string(*raw);
}

#endif

