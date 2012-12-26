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

namespace Game { namespace io { namespace sample {

    ALLEGRO_SAMPLE *open(std::string filename) {

        debugArgs("io::sample", "Loading '%s'...", filename.data());

        ALLEGRO_SAMPLE *sample = NULL;
        ALLEGRO_FILE *file  = file::open(filename);

        if (file != NULL) {
            std::string ext = filename.substr(filename.find_last_of("."));
            sample = al_load_sample_f(file, ext.data());
        } 
        
        if (sample) {
            debugArgs("io::sample", "Loaded '%s'", filename.data());

        } else {
            debugArgs("io::sample", "Failed to load '%s'", filename.data());
        }

        return sample;

    }

}}}

