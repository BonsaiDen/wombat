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

namespace Game { namespace io { namespace file {

    // TODO add switch for memory / disk files
    // and mopve the al_open_memfile here
    void *load(std::string filename, int64_t *len) {
        
        ALLEGRO_FILE* file = al_fopen(filename.data(), "r");
        if (file) {
            int64_t size = al_fsize(file);
            void* buffer = al_malloc((size_t)size);
            al_fread(file, buffer, size);
            al_fclose(file);

            *len = size;
            return buffer;

        } else {
            *len = -1;
            return NULL;
        }

    }

    ALLEGRO_FILE *open(std::string filename) {

        debugArgs("io::file", "Loading '%s'...", filename.data());

        int64_t len;
        void *buf = load(filename, &len);

        if (buf == NULL) {
            debugArgs("io::file", "Failed to load '%s'", filename.data());
            return NULL;

        } else {
            debugArgs("io::file", "Loaded '%s', %d bytes", filename.data(), (int)len);
            return al_open_memfile(buf, len, "r");
        }

    }

    bool close(ALLEGRO_FILE *fp) {

        if (fp != NULL) {
            debugMsg("io::file", "Closed file");
            al_fclose(fp);
            return true;

        } else {
            return false;
        }

    }

}}}

