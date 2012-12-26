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
#include <fstream>

namespace Game { namespace io { namespace file {

    char *loadResource(std::string filename, unsigned int *bufSize) {
        
        std::ifstream scriptFile;
        scriptFile.open(filename.data(), std::ifstream::in);

        if (scriptFile.is_open()) {

            scriptFile.seekg(0, std::ios::end);
            std::ifstream::pos_type size = scriptFile.tellg();
            
            char *data = new char[size];
            scriptFile.seekg(0, std::ios::beg);
            scriptFile.read(data, size);
            scriptFile.close();

            *bufSize = size;
            return data;

        } else {
            *bufSize = -1;
            return NULL;
        }

    }

    ALLEGRO_FILE *open(std::string filename) {

        debugArgs("io::file", "Loading '%s'...", filename.data());

        unsigned int len;
        char *buf = loadResource(filename, &len);

        if (buf == NULL) {
            debugArgs("io::file", "Failed to load '%s'", filename.data());
            return NULL;

        } else {
            debugArgs("io::file", "Loaded '%s', %d bytes", filename.data(), len);
            return al_open_memfile(buf, len, "r");
        }

    }

}}}

