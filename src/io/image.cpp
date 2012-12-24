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

using namespace std;
namespace Game { namespace io { namespace image {

    ALLEGRO_BITMAP *open(string filename) {

        printf("[io::image] Loading \"%s\"...\n", filename.data());

        ALLEGRO_BITMAP *img = NULL;
        ALLEGRO_FILE *file  = file::open(filename);

        if (file != NULL) {
            string ext = filename.substr(filename.find_last_of("."));
            img = al_load_bitmap_f(file, ext.data());
        } 
        
        if (img) {
            printf("[io::image] Loaded \"%s\"\n", filename.data());

        } else {
            printf("[io::image] Failed to load \"%s\"\n", filename.data());
        }

        return img;

    }

}}}

