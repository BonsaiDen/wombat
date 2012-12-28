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
#include "js.h"

#include <v8.h>
#include <fstream>
#include <string>
#include <stdio.h>

v8::Persistent<v8::Object> JSObject() {
    return v8::Persistent<v8::Object>::New(v8::ObjectTemplate::New()->NewInstance());
}

v8::Handle<v8::Value> requireScript(const v8::Arguments& args) {

    v8::HandleScope scope;
    if (args.Length() == 1) {
        v8::String::Utf8Value name(args[0]->ToString());
        return scope.Close(executeScript(loadScript(*name)));
    
    } else {
        return v8::Undefined();
    }

}

v8::Handle<v8::Value> executeScript(v8::Handle<v8::Script> script) {

    v8::HandleScope scope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = script->Run();

    if (result.IsEmpty()) {
        handleException(tryCatch);
        return v8::Undefined();
    
    } else {
        return scope.Close(result);
    }

}

v8::Handle<v8::Script> loadScript(const char *name) {
    
    // Create filename
    std::string filename(name);
    filename.append(".js");
    
    // Handles
    v8::HandleScope scope;
    v8::Handle<v8::Script> script;
    v8::Handle<v8::String> source;
    v8::TryCatch tryCatch;
    
    // Load File
    std::ifstream scriptFile;
    scriptFile.open(filename.data(), std::ifstream::in);

    if (scriptFile.is_open()) {
        scriptFile.seekg(0, std::ios::end);
        std::ifstream::pos_type size = scriptFile.tellg();
        
        char *data = new char[size];
        scriptFile.seekg(0, std::ios::beg);
        scriptFile.read(data, size);
        scriptFile.close();
        source = v8::String::New(data, size);
        delete[] data;
    
    } else {
        printf("Fatal Error: Failed to load module \"%s\"\n", name);
        script = v8::Script::Compile(v8::String::New("(function() { var module = { exports: {} }; (function(exports, module, global) {})(module.exports, module, this); return module; })();"), v8::String::New(filename.data()));
        return scope.Close(script);
    }
    
    // Wrap it in a Node.js compatible way
    v8::Handle<v8::String> pre = v8::String::New("(function() { var module = { exports: {} }; (function(exports, module, global) {\n");
    v8::Handle<v8::String> end = v8::String::New("\n})(module.exports, module, this); return module; })();");
    v8::Handle<v8::String> wrapped = v8::String::Concat(v8::String::Concat(pre, source), end);

    script = v8::Script::Compile(wrapped, v8::String::New(filename.data()));
    if (script.IsEmpty()) {
        printf("Fatal Error: Could not load module \"%s\"\n", name);
        handleException(tryCatch);
        script = v8::Script::Compile(v8::String::New("undefined"), v8::String::New(filename.data()));
    }

    return scope.Close(script);

}

void handleException(v8::TryCatch tryCatch) {

    v8::HandleScope scope;
    v8::String::Utf8Value exception(tryCatch.Exception());

    v8::Handle<v8::Message> message = tryCatch.Message();
    if (message.IsEmpty()) {
        printf("%s\n", *exception);
    
    } else {
    
        // Filename, line, message
        v8::String::Utf8Value filename(message->GetScriptResourceName());
        int line = message->GetLineNumber();
        printf("\nError at %s:%i:\n  %s\n", *filename, line - 1, *exception); // -1 for the wrapper
        
        // Sourceline
        v8::String::Utf8Value sourceLine(message->GetSourceLine());
        printf("\nSource:\n  %s\n", *sourceLine);
        
        int start = message->GetStartColumn(), end = message->GetEndColumn();
        for(int i = 0; i < start; i++) {
            printf(" ");
        }
        for(int i = start; i < end; i++) {
            printf("^");
        }
        printf("\n");
        
        // Stack Trace
        //String::Utf8Value stacktrace(TryCatch.StackTrace());
        //if (stacktrace.length() > 0) {
            //printf("%s\n", *stacktrace);
        //}
        
    }

}

