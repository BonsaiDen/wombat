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

namespace Game { namespace api { namespace graphics {

    #define getNumber(obj, key)

    #define colorMap(colorValue) \
        int l = args.Length(); \
        int a = l > 3 ? (int)(ToFloat(args[3]) * 255) : 255; \
        if (l >= 3) { \
            colorValue = al_map_rgba(ToInt32(args[0]), ToInt32(args[1]), ToInt32(args[2]), a); \
        } \
        return v8::Undefined();

    #define colorUnmap(colorValue) \
        unsigned char r, g, b, a; \
        al_unmap_rgba(colorValue, &r, &g, &b, &a); \
        \
        v8::Handle<v8::Object> col = templates.color->NewInstance(); \
        setNumberProp(col, "r", r); \
        setNumberProp(col, "g", g); \
        setNumberProp(col, "b", b); \
        setNumberProp(col, "a", ((double)a) / 255); \
        return col;
        
    #define colorMapFloat(colorValue) \
        int l = args.Length(); \
        float a = l > 3 ? ToFloat(args[3]) : 1; \
        if (l >= 3) { \
            colorValue = al_map_rgba_f(ToFloat(args[0]), ToFloat(args[1]), ToFloat(args[2]), a); \
        } \
        return v8::Undefined();

    #define colorUnmapFloat(colorValue) \
        float r, g, b, a; \
        al_unmap_rgba_f(colorValue, &r, &g, &b, &a); \
        \
        v8::Handle<v8::Object> col = templates.color->NewInstance(); \
        setNumberProp(col, "r", r); \
        setNumberProp(col, "g", g); \
        setNumberProp(col, "b", b); \
        setNumberProp(col, "a", a); \
        return col;



    // API --------------------------------------------------------------------
    v8::Handle<v8::Value> setRenderOffset(const v8::Arguments& args) {
        return v8::Undefined();
    }
    v8::Handle<v8::Value> getRenderOffset(const v8::Arguments& args) {
        v8::Handle<v8::Object> pos = templates.position->NewInstance();
        setNumberProp(pos, "x", Game::graphics.offsetX);
        setNumberProp(pos, "y", Game::graphics.offsetY);
        return pos;
    }

    v8::Handle<v8::Value> setScale(const v8::Arguments& args) {
        if (args.Length() >= 1) {
            int scale = ToInt32(args[0]);
            if (scale != Game::graphics.scale) {
                setNumberProp(js.config, "scale", scale);
                Game::graphics.scale = scale;
                Game::graphics.wasResized = true;
            }
        }
        return v8::Undefined();
    }
    v8::Handle<v8::Value> getScale(const v8::Arguments& args) {
        return v8::Number::New(Game::graphics.scale);
    }

    v8::Handle<v8::Value> setSize(const v8::Arguments& args) {
        if (args.Length() >= 2) {
            int width = ToInt32(args[0]);
            int height = ToInt32(args[1]);
            if (width != Game::graphics.width || height != Game::graphics.height) {
                setNumberProp(js.config, "width", width);
                setNumberProp(js.config, "height", height);
                Game::graphics.width = width;
                Game::graphics.height = height;
                Game::graphics.wasResized = true;
            }
        }
        return v8::Undefined();
    }
    v8::Handle<v8::Value> getSize(const v8::Arguments& args) {
        v8::Handle<v8::Object> size = templates.size->NewInstance();
        setNumberProp(size, "w", Game::graphics.width);
        setNumberProp(size, "h", Game::graphics.height);
        return size;
    }


    // Colors ---------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    v8::Handle<v8::Value> setColor(const v8::Arguments& args) {
        colorMap(Game::graphics.color);
    }
    v8::Handle<v8::Value> getColor(const v8::Arguments& args) {
        colorUnmap(Game::graphics.color);
    }


    v8::Handle<v8::Value> setBackColor(const v8::Arguments& args) {
        colorMap(Game::graphics.bgColor);
    }
    v8::Handle<v8::Value> getBackColor(const v8::Arguments& args) {
        colorUnmap(Game::graphics.bgColor);
    }


    v8::Handle<v8::Value> setBlendColor(const v8::Arguments& args) {
        colorMapFloat(Game::graphics.blendColor);
    }
    v8::Handle<v8::Value> getBlendColor(const v8::Arguments& args) {
        colorUnmapFloat(Game::graphics.blendColor);
    }


    // Primitives -----------------------------------------------------------------
    // ----------------------------------------------------------------------------
    v8::Handle<v8::Value> setLineWidth(const v8::Arguments& args) {
        if (args.Length() >= 1) {
            Game::graphics.lineWidth = ToInt32(args[0]);
        }
        return v8::Undefined();
    }

    v8::Handle<v8::Value> getLineWidth(const v8::Arguments& args) {
        return v8::Number::New(Game::graphics.lineWidth);
    }

    v8::Handle<v8::Value> line(const v8::Arguments& args) {

        if (args.Length() >= 4) {

            double x1 = ToFloat(args[0]) + Game::graphics.offsetX;
            double y1 = ToFloat(args[1]) + Game::graphics.offsetY;
            double x2 = ToFloat(args[2]) + Game::graphics.offsetX;
            double y2 = ToFloat(args[3]) + Game::graphics.offsetY;

            if (Game::graphics.lineWidth % 2 == 1) {
                x1 += 0.5;
                x2 += 0.5;
            } 

            al_draw_line(x1, y1, x2, y2, Game::graphics.color, Game::graphics.lineWidth);
            
        }

        return v8::Undefined();

    }

    v8::Handle<v8::Value> rect(const v8::Arguments& args) {

        if (args.Length() >= 4) {

            double x = ToFloat(args[0]) + Game::graphics.offsetX;
            double y = ToFloat(args[1]) + Game::graphics.offsetY;
            double w = ToFloat(args[2]) + Game::graphics.offsetX;
            double h = ToFloat(args[3]) + Game::graphics.offsetY;

            if (args.Length() >= 5 && args[4]->BooleanValue() == true) {
                al_draw_filled_rectangle(x, y, x + w, y + h, Game::graphics.color);

            } else {

                if (Game::graphics.lineWidth % 2 == 1) {
                    x += 0.5;
                    y += 0.5;
                } 

                al_draw_rectangle(x, y, x + w - 1, y + h - 1, Game::graphics.color, Game::graphics.lineWidth);

            }

        }

        return v8::Undefined();

    }

    // Export -----------------------------------------------------------------
    void init(const v8::Handle<v8::Object> &object) {

        setFunctionProp(object, "setRenderOffset", setRenderOffset);
        setFunctionProp(object, "getRenderOffset", getRenderOffset);

        setFunctionProp(object, "setScale", setScale);
        setFunctionProp(object, "getScale", getScale);

        setFunctionProp(object, "setSize", setSize);
        setFunctionProp(object, "getSize", getSize);

        setFunctionProp(object, "setColor", setColor);
        setFunctionProp(object, "getColor", getColor);

        setFunctionProp(object, "setBackColor", setBackColor);
        setFunctionProp(object, "getBackColor", getBackColor);

        setFunctionProp(object, "setBlendColor", setBlendColor);
        setFunctionProp(object, "getBlendColor", getBlendColor);

        setFunctionProp(object, "setLineWidth", setLineWidth);
        setFunctionProp(object, "getLineWidth", getLineWidth);

        setFunctionProp(object, "line", line);
        setFunctionProp(object, "rect", rect);

    }

}}}

