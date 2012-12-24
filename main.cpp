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
#include <stdio.h>
#include "src/Game.h"

using namespace v8;

int main(int argc, char* argv[]) {
    
    //setvbuf(stdout, NULL, _IOFBF, 8192);

    // Setup V8
    HandleScope scope;
    Persistent<Context> context = Context::New();
    Context::Scope contextScope(context);

    int ret = 0;
    if (Game::init(context)) {
        if (Game::start(argc > 1 ? string(argv[1]) : "")) {
            ret = Game::loop();
        }

    } else {
        ret = 1;
    }

    context.Dispose();
    return ret;

}

