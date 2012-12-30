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

namespace Game { namespace api { namespace console {

    // API --------------------------------------------------------------------
    v8::Handle<v8::Value> log(const v8::Arguments& args) {

        v8::HandleScope scope;
        v8::Local<v8::Function> callee = args.Callee();
          
        bool first = true;

        printf("[console::log] ");
        for(int i = 0; i < args.Length(); i++) {

            if (first) {
                first = false;
            
            } else {
                printf(" ");
            }

            v8::String::Utf8Value str(args[i]);
            if (args[i]->IsArray()) {
                printf("[ %s ]", *str);
                
            } else if (args[i]->IsFunction()) {
                printf("%s", *str);
                
            } else if (args[i]->IsObject()) {

                printf("{ ");

                v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(args[i]);
                v8::Handle<v8::Array> keys = object->GetPropertyNames();
                for (unsigned int i = 0; i < keys->Length(); i++) {

                    v8::Handle<v8::String> name = keys->Get(v8::Number::New(i))->ToString();
                    v8::Handle<v8::Value> value = object->Get(name)->ToString();
                    v8::String::Utf8Value key(name);
                    v8::String::Utf8Value val(value);

                    printf("%s: %s ", *key, *val);

                    if (i < keys->Length() - 1) {
                        printf(", ");
                    }

                }

                printf(" }");
                
            } else {
                printf("%s", *str);
            }

        }

        printf("\n");
        return v8::Undefined();

    }


    // Export -----------------------------------------------------------------
    void init(const v8::Handle<v8::Object> &object) {
        setFunctionProp(object, "log", log);
    }

}}}

