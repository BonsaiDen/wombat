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
#include <v8.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include "js.h"

using namespace v8;
using namespace std;

Persistent<Object> JSObject() {
    return Persistent<Object>::New(ObjectTemplate::New()->NewInstance());
}

Handle<Value> requireScript(const Arguments& args) {

    HandleScope scope;
    if (args.Length() == 1) {
        String::Utf8Value name(args[0]->ToString());
        return scope.Close(executeScript(loadScript(*name)));
    
    } else {
        return Undefined();
    }

}

Handle<Value> executeScript(Handle<Script> script) {

    HandleScope scope;
    TryCatch tryCatch;
    Handle<Value> result = script->Run();

    if (result.IsEmpty()) {
        handleException(tryCatch);
        return Undefined();
    
    } else {
        return scope.Close(result);
    }

}

Handle<Script> loadScript(const char *name) {
    
    // Create filename
    string filename(name);
    filename.append(".js");
    
    // Handles
    HandleScope scope;
    Handle<Script> script;
    Handle<String> source;
    TryCatch tryCatch;
    
    // Load File
    ifstream scriptFile;
    scriptFile.open(filename.data(), ifstream::in);

    if (scriptFile.is_open()) {
        scriptFile.seekg(0, ios::end);
        ifstream::pos_type size = scriptFile.tellg();
        
        char *data = new char[size];
        scriptFile.seekg(0, ios::beg);
        scriptFile.read(data, size);
        scriptFile.close();
        source = String::New(data, size);
        delete[] data;
    
    } else {
        printf("Error: Failed to load module \"%s\"\n", name);
        script = Script::Compile(String::New("undefined"), String::New(filename.data()));
        return scope.Close(script);
    }
    
    // Wrap it in a Node.js compatible way
    Handle<String> pre = String::New("(function() {var exports = {}; (function(exports, global) {\n");
    Handle<String> end = String::New("\n})(exports, this); return exports;})();");
    Handle<String> wrapped = String::Concat(String::Concat(pre, source), end);

    script = Script::Compile(wrapped, String::New(filename.data()));
    if (script.IsEmpty()) {
        printf("Error: Could not load module \"%s\"\n", name);
        handleException(tryCatch);
        script = Script::Compile(String::New("undefined"), String::New(filename.data()));
    }

    return scope.Close(script);

}

void handleException(TryCatch tryCatch) {

    HandleScope scope;
    String::Utf8Value exception(tryCatch.Exception());

    Handle<Message> message = tryCatch.Message();
    if (message.IsEmpty()) {
        printf("%s\n", *exception);
    
    } else {
    
        // Filename, line, message
        String::Utf8Value filename(message->GetScriptResourceName());
        int line = message->GetLineNumber();
        printf("\nError at %s:%i:\n  %s\n", *filename, line - 1, *exception); // -1 for the wrapper
        
        // Sourceline
        String::Utf8Value sourceLine(message->GetSourceLine());
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
        //String::Utf8Value stackTrace(tryCatch.StackTrace());
        //if (stackTrace.length() > 0) {
            //printf("%s\n", *stackTrace);
        //}
        
    }

}

