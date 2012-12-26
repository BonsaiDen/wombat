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

using namespace v8;
namespace Game { namespace api { namespace image {

    // Structs ----------------------------------------------------------------
    struct Image;
    typedef struct Image {
        std::string filename;
        ALLEGRO_BITMAP *bitmap;
        bool loaded;
        int cols;
        int rows;

    } Image;

    typedef std::map<const std::string, Image*> ImageMap;


    // Loader -----------------------------------------------------------------
    ImageMap *images;
    Image* getImage(std::string filename, const int cols, const int rows) {
        
        ImageMap::iterator it = images->find(filename);
        if (it == images->end()) {
            
            Image *img = new Image();
            img->filename = filename;
            img->bitmap = io::image::open(filename);
            img->loaded = img->bitmap != NULL;
            img->cols = cols;
            img->rows = rows;
            images->insert(std::make_pair(filename, img));

            return img;

        } else {
            return it->second;
        }

    }

    // API --------------------------------------------------------------------
    Handle<Value> load(const Arguments& args) {

        if (args.Length() > 0) {
                
            int rows = 1;
            int cols = 1;

            if (args.Length() >= 3) {
                cols = ToInt32(args[1]);
                rows = ToInt32(args[2]);
            }

            if (getImage(ToString(args[0]), rows, cols)->loaded) {   
                return True();
            }

        } 

        return False();

    }

    Handle<Value> draw(const Arguments& args) {

        if (args.Length() < 3) {
            return Undefined();
        }

        Image *img = getImage(ToString(args[0]), 1, 1);
        if (img->bitmap == NULL) {
            return False();
        }

        int x = ToInt32(args[1]) + Game::graphics.offsetX; 
        int y = ToInt32(args[2]) + Game::graphics.offsetY; 

        int flags = 0;
        if (args.Length() > 3 && args[3]->BooleanValue() == true) {
            flags |= ALLEGRO_FLIP_HORIZONTAL;
        }

        if (args.Length() > 4 && args[4]->BooleanValue() == true) {
            flags |= ALLEGRO_FLIP_VERTICAL;
        }

        double a = args.Length() > 5 ? ToFloat(args[5]) : 1;
        if (a == 1) {
            al_draw_bitmap(img->bitmap, x, y, flags);

        } else {
            al_draw_tinted_bitmap(img->bitmap, al_map_rgba_f(1, 1, 1, a), x, y, flags);
        }

        return True();

    }

    Handle<Value> setTiled(const Arguments& args) {

        if (args.Length() >= 3) {

            int cols = ToInt32(args[1]);
            int rows = ToInt32(args[2]);

            Image *img = getImage(ToString(args[0]), cols, rows);
            img->cols = cols;
            img->rows = rows;
            
        }

        return Undefined();

    }

    Handle<Value> drawTiled(const Arguments& args) {

        if (args.Length() < 4) {
            return Undefined();
        }

        Image *img = getImage(ToString(args[0]), 1, 1);
        if (img->bitmap == NULL) {
            return False();
        }

        int x = ToInt32(args[1]) + Game::graphics.offsetX; 
        int y = ToInt32(args[2]) + Game::graphics.offsetY; 
        int index = ToInt32(args[3]);

        int w = al_get_bitmap_width(img->bitmap) / img->cols;
        int h = al_get_bitmap_height(img->bitmap) / img->rows;
        int ty = index / img->rows;
        int tx = index - ty * img->cols;

        int flags = 0;
        if (args.Length() > 5 && args[4]->BooleanValue() == true) {
            flags |= ALLEGRO_FLIP_HORIZONTAL;
        }

        if (args.Length() > 6 && args[5]->BooleanValue() == true) {
            flags |= ALLEGRO_FLIP_VERTICAL;
        }

        double a = args.Length() > 7 ? ToFloat(args[6]) : 1;
        if (a == 1) {
            al_draw_bitmap_region(img->bitmap, tx * w, ty * h, w, h, x, y, flags);

        } else {
            al_draw_tinted_bitmap_region(img->bitmap, al_map_rgba_f(1, 1, 1, a), tx * w, ty * h, w, h, x, y, flags);
        }

        return True();

    }


    // Export -----------------------------------------------------------------
    void init(Handle<Object> object) {

        images = new ImageMap();

        setFunctionProp(object, "load", load);
        setFunctionProp(object, "draw", draw);
        setFunctionProp(object, "drawTiled", drawTiled);
        setFunctionProp(object, "setTiled", setTiled);

    }

    void shutdown() {
        
        debugMsg("api::image", "Shutdown...");
        for(ImageMap::iterator it = images->begin(); it != images->end(); it++) {
            Image *img = it->second;
            if (img->bitmap) {
                debugArgs("api::image::bitmap", "Destroyed '%s'", img->filename.data());
                al_destroy_bitmap(img->bitmap);
            }
            debugArgs("api::image", "Destroyed '%s'", img->filename.data());
            delete img;
        }

        images->clear();
        delete images;

    }

}}}

