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

namespace Game { namespace api { namespace keyboard {
        
    static const char *names[ALLEGRO_KEY_MAX] = { 
       "(none)",      "KEY_A",       "KEY_B",      "KEY_C",
       "KEY_D",       "KEY_E",       "KEY_F",      "KEY_G",
       "KEY_H",       "KEY_I",       "KEY_J",      "KEY_K",
       "KEY_L",       "KEY_M",       "KEY_N",      "KEY_O",
       "KEY_P",       "KEY_Q",       "KEY_R",      "KEY_S",
       "KEY_T",       "KEY_U",       "KEY_V",      "KEY_W",
       "KEY_X",       "KEY_Y",       "KEY_Z",      "KEY_0",
       "KEY_1",       "KEY_2",       "KEY_3",      "KEY_4",
       "KEY_5",       "KEY_6",       "KEY_7",      "KEY_8",

       "KEY_9",       "PAD_0",       "PAD_1",      "PAD_2",
       "PAD_3",       "PAD_4",       "PAD_5",      "PAD_6",
       "PAD_7",       "PAD_8",       "PAD_9",      "F1",
       "F2",          "F3",          "F4",         "F5",
       "F6",          "F7",          "F8",         "F9",
       "F10",         "F11",         "F12",        "ESCAPE",
       "KEY_60",      "KEY_61",      "KEY_62",     "BACKSPACE",
       "TAB",         "KEY_65",      "KEY_66",     "ENTER",

       "KEY_68",      "KEY_69",      "BACKSLASH",  "KEY_71",
       "KEY_72",      "KEY_73",      "KEY_74",     "SPACE",
       "INSERT",      "DELETE",      "HOME",       "END",
       "PGUP",        "PGDN",        "LEFT",       "RIGHT",
       "UP",          "DOWN",        "PAD_DIV",    "PAD_MUL",

       "PAD_MINUS",   "PAD_PLUS",    "PAD_DEL",    "PAD_ENTER",

       "PRINTSCREEN", "PAUSE",       "KEY_94",     "KEY_95",
       "KEY_96",      "KEY_97",      "KEY_98",     "KEY_99",
       "KEY_100",     "KEY_101",     "KEY_102",    "PAD_EQ",
       "KEY_104",     "KEY_105",     "KEY_106",    "KEY_107",
       "KEY_108",     "KEY_109",     "KEY_110",    "KEY_111",
       "KEY_112",     "KEY_113",     "KEY_114",    "KEY_115",
       "KEY_116",     "KEY_117",     "KEY_118",    "KEY_119",
       "KEY_120",     "KEY_121",     "KEY_122",    "KEY_123",
       "KEY_124",     "KEY_125",     "KEY_126",    "KEY_127",
       "KEY_128",     "KEY_129",     "KEY_130",    "KEY_131",
       "KEY_132",     "KEY_133",     "KEY_134",    "KEY_135",
       "KEY_136",     "KEY_137",     "KEY_138",    "KEY_139",
       "KEY_140",     "KEY_141",     "KEY_142",    "KEY_143",
       "KEY_144",     "KEY_145",     "KEY_146",    "KEY_147",
       "KEY_148",     "KEY_149",     "KEY_150",    "KEY_151",
       "KEY_152",     "KEY_153",     "KEY_154",    "KEY_155",
       "KEY_156",     "KEY_157",     "KEY_158",    "KEY_159",
       "KEY_160",     "KEY_161",     "KEY_162",    "KEY_163",
       "KEY_164",     "KEY_165",     "KEY_166",    "KEY_167",
       "KEY_168",     "KEY_169",     "KEY_170",    "KEY_171",
       "KEY_172",     "KEY_173",     "KEY_174",    "KEY_175",
       "KEY_176",     "KEY_177",     "KEY_178",    "KEY_179",
       "KEY_180",     "KEY_181",     "KEY_182",    "KEY_183",
       "KEY_184",     "KEY_185",     "KEY_186",    "KEY_187",
       "KEY_188",     "KEY_189",     "KEY_190",    "KEY_191",
       "KEY_192",     "KEY_193",     "KEY_194",    "KEY_195",
       "KEY_196",     "KEY_197",     "KEY_198",    "KEY_199",
       "KEY_200",     "KEY_201",     "KEY_202",    "KEY_203",
       "KEY_204",     "KEY_205",     "KEY_206",    "KEY_207",
       "KEY_208",     "KEY_209",     "KEY_210",    "KEY_211",
       "KEY_212",     "KEY_213",     "KEY_214",    "LSHIFT",
       "RSHIFT",      "LCTRL",       "RCTRL",      "ALT",
       "ALTGR",       "LWIN",        "RWIN",       "MENU",
       "SCROLLLOCK",  "NUMLOCK",     "CAPSLOCK"
    };


    // API --------------------------------------------------------------------
    v8::Handle<v8::Value> isDown(const v8::Arguments& args) {
        
        bool ok = false;
        if (args.Length() > 0) {
            int id = ToInt32(args[0]);
            if (id > 0 && id < 227) {
                ok = Game::keyboard.state[id] > 0;
            }
        }

        return ok? v8::True() : v8::False();

    }

    v8::Handle<v8::Value> wasPressed(const v8::Arguments& args) {

        bool ok = false;
        if (args.Length() > 0) {
            int id = ToInt32(args[0]);
            if (id > 0 && id < 227) {
                ok = Game::keyboard.state[id] == 1;
            }
        }

        return ok? v8::True() : v8::False();

    }

    v8::Handle<v8::Value> wasReleased(const v8::Arguments& args) {

        bool ok = false;
        if (args.Length() > 0) {
            int id = ToInt32(args[0]);
            if (id > 0 && id < 227) {
                ok = Game::keyboard.state[id] == 0 && Game::keyboard.stateOld[id] > 0;
            }
        }

        return ok? v8::True() : v8::False();

    }

    v8::Handle<v8::Value> hasFocus(const v8::Arguments& args) {
        return Game::keyboard.hasFocus ? v8::True() : v8::False();
    }

    v8::Handle<v8::Value> getCount(const v8::Arguments& args) {
        return v8::Number::New(Game::keyboard.pressedCount);
    }


    // Export -----------------------------------------------------------------
    void init(v8::Handle<v8::Object> object) {

        for(unsigned int i = 1; i < 227; i++) {
            setNumberProp(object, names[i], i);
        }

        setFunctionProp(object, "isDown", isDown);
        setFunctionProp(object, "wasPressed", wasPressed);
        setFunctionProp(object, "wasReleased", wasReleased);
        setFunctionProp(object, "hasFocus", hasFocus);
        setFunctionProp(object, "getCount", getCount);

    }

}}}

