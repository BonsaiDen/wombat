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
namespace Game { namespace api { namespace graphics {

    #define getNumber(obj, key)

    #define colorMap(colorValue) \
        int l = args.Length(); \
        int a = l > 3 ? (int)(ToFloat(args[3]) * 255) : 255; \
        if (l >= 3) { \
            colorValue = al_map_rgba(ToInt32(args[0]), ToInt32(args[1]), ToInt32(args[2]), a); \
        } \
        return Undefined();

    #define colorUnmap(colorValue) \
        unsigned char r, g, b, a; \
        al_unmap_rgba(colorValue, &r, &g, &b, &a); \
        \
        Handle<Object> col = templates.color->NewInstance(); \
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
        return Undefined();

    #define colorUnmapFloat(colorValue) \
        float r, g, b, a; \
        al_unmap_rgba_f(colorValue, &r, &g, &b, &a); \
        \
        Handle<Object> col = templates.color->NewInstance(); \
        setNumberProp(col, "r", r); \
        setNumberProp(col, "g", g); \
        setNumberProp(col, "b", b); \
        setNumberProp(col, "a", a); \
        return col;



    // API --------------------------------------------------------------------
    Handle<Value> setRenderOffset(const Arguments& args) {
        return Undefined();
    }
    Handle<Value> getRenderOffset(const Arguments& args) {
        Handle<Object> pos = templates.position->NewInstance();
        setNumberProp(pos, "x", Game::graphics.offsetX);
        setNumberProp(pos, "y", Game::graphics.offsetY);
        return pos;
    }

    Handle<Value> setScale(const Arguments& args) {
        if (args.Length() >= 1) {
            int scale = ToInt32(args[0]);
            if (scale != Game::graphics.scale) {
                Game::graphics.scale = scale;
                Game::graphics.wasResized = true;
            }
        }
        return Undefined();
    }
    Handle<Value> getScale(const Arguments& args) {
        return Number::New(Game::graphics.scale);
    }

    Handle<Value> setSize(const Arguments& args) {
        if (args.Length() >= 2) {
            int width = ToInt32(args[0]);
            int height = ToInt32(args[1]);
            if (width != Game::graphics.width || height != Game::graphics.height) {
                Game::graphics.width = width;
                Game::graphics.height = height;
                Game::graphics.wasResized = true;
            }
        }
        return Undefined();
    }
    Handle<Value> getSize(const Arguments& args) {
        Handle<Object> size = templates.size->NewInstance();
        setNumberProp(size, "w", Game::graphics.width);
        setNumberProp(size, "h", Game::graphics.height);
        return size;
    }


    // Colors ---------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    Handle<Value> setColor(const Arguments& args) {
        colorMap(Game::graphics.color);
    }
    Handle<Value> getColor(const Arguments& args) {
        colorUnmap(Game::graphics.color);
    }


    Handle<Value> setBackColor(const Arguments& args) {
        colorMap(Game::graphics.bgColor);
    }
    Handle<Value> getBackColor(const Arguments& args) {
        colorUnmap(Game::graphics.bgColor);
    }


    Handle<Value> setBlendColor(const Arguments& args) {
        colorMapFloat(Game::graphics.blendColor);
    }
    Handle<Value> getBlendColor(const Arguments& args) {
        colorUnmapFloat(Game::graphics.blendColor);
    }


    // Primitives -----------------------------------------------------------------
    // ----------------------------------------------------------------------------
    Handle<Value> setLineWidth(const Arguments& args) {
        if (args.Length() >= 1) {
            Game::graphics.lineWidth = ToInt32(args[0]);
        }
        return Undefined();
    }

    Handle<Value> getLineWidth(const Arguments& args) {
        return Number::New(Game::graphics.lineWidth);
    }

    Handle<Value> line(const Arguments& args) {

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

        return Undefined();

    }

    Handle<Value> rect(const Arguments& args) {

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

        return Undefined();

    }

    // Export -----------------------------------------------------------------
    void init(Handle<Object> object) {

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

