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
namespace Game { namespace api { namespace music {

    // Structs ----------------------------------------------------------------
    typedef enum MUSIC_STATE {
        MUSIC_STATE_STOPPED = 0,
        MUSIC_STATE_PLAYING = 1,
        MUSIC_STATE_PAUSED = 2
        
    } MUSIC_STATE;

    struct Music;
    typedef struct Music {
        std::string filename;
        ALLEGRO_AUDIO_STREAM *stream;
        bool loaded;
        bool looping;
        MUSIC_STATE state;

    } Music;

    typedef std::map<const std::string, Music*> MusicMap;


    // Macros -----------------------------------------------------------------
    #define mapMusic(a, m) \
        if (a.Length() > 0) { \
            m = getMusic(ToString(a[0])); \
            if (!m->loaded) { \
                m = NULL; \
            } \
        } \
    
    #define musicState(cmp, newState, action) \
        mapMusic(args, music); \
        if (music) { \
            if (cmp) { \
                music->state = newState; \
                action; \
                return True(); \
            \
            } else { \
                return False(); \
            } \
            \
        } else { \
            return Undefined(); \
        }


    // Loader -----------------------------------------------------------------
    MusicMap *musics;
    Music* getMusic(std::string filename) {
        
        MusicMap::iterator it = musics->find(filename);
        if (it == musics->end()) {
            
            Music *music = new Music();
            music->stream = io::stream::open(filename);
            music->loaded = music->stream != NULL;

            if (music->stream == NULL) {
                printf("[api::music] Failed to load \"%s\"\n", filename.data());

            } else {
                al_set_audio_stream_playing(music->stream, false);
            }

            music->looping = false;
            music->state = MUSIC_STATE_STOPPED;

            musics->insert(std::make_pair(filename, music));

            return music;

        } else {
            return it->second;
        }

    }
    void attachMusicStream(Music *music) {
        if (music->stream && !al_get_audio_stream_attached(music->stream)) {
            printf("attach music...\n");
            al_attach_audio_stream_to_mixer(music->stream, al_get_default_mixer());
        }
    }

    void detachMusicStream(Music *music) {
        if (music->stream && al_get_audio_stream_attached(music->stream)) {
            printf("detach music...\n");
            al_detach_audio_stream(music->stream);
        }
    }


    // API --------------------------------------------------------------------
    Handle<Value> load(const Arguments& args) {

        Music *music;
        mapMusic(args, music);
        if (music) {
            return v8::True();
        } 

        return v8::False();

    }


    // Play Controls ----------------------------------------------------------
    Handle<Value> play(const Arguments& args) {

        Music *music;
        musicState(
            music->state != MUSIC_STATE_PLAYING,
            MUSIC_STATE_PLAYING, 
            attachMusicStream(music);
            printf("play music, rewind...\n");
            al_rewind_audio_stream(music->stream);
            al_set_audio_stream_playing(music->stream, true);
        )
        
    }

    Handle<Value> pause(const Arguments& args) {

        Music *music;
        musicState(
            music->state == MUSIC_STATE_PLAYING,
            MUSIC_STATE_PAUSED, 

            al_set_audio_stream_playing(music->stream, false);
        )

    }

    Handle<Value> resume(const Arguments& args) {

        Music *music;
        musicState(
            music->state == MUSIC_STATE_PAUSED,
            MUSIC_STATE_PLAYING, 
            al_set_audio_stream_playing(music->stream, true);
        )

    }

    Handle<Value> stop(const Arguments& args) {
        
        Music *music;
        musicState(
            music->state != MUSIC_STATE_STOPPED,
            MUSIC_STATE_STOPPED,
            printf("stop music, drain, rewind...\n");
            detachMusicStream(music);
            al_drain_audio_stream(music->stream);
            al_set_audio_stream_playing(music->stream, false);
        )

    }

    Handle<Value> setLooping(const Arguments& args) {

        Music *music;
        mapMusic(args, music);
        if (music && args.Length() > 1) {

            bool loop = ToBoolean(args[1]);
            if (loop != music->looping) {
                al_set_audio_stream_playmode(music->stream, loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);
                music->looping = loop;
                return True();
                
            } else {
                return False();
            }
            
        } else {
            return Undefined();    
        }

    }

    Handle<Value> setPan(const Arguments& args) {
        return Undefined();
    }

    Handle<Value> setVolume(const Arguments& args) {
        return Undefined();
    }

    // Export -----------------------------------------------------------------
    void init(Handle<Object> object) {

        musics = new MusicMap();
        setFunctionProp(object, "load", load);
        setFunctionProp(object, "play", play);
        setFunctionProp(object, "pause", pause);
        setFunctionProp(object, "resume", resume);
        setFunctionProp(object, "stop", stop);
        setFunctionProp(object, "setLooping", setLooping);

    }

    void update(double time, double dt) {
        
    }

    void shutdown() {
        debugMsg("api::music", "Shutdown...");
        musics->clear();
        delete musics;
    }

}}}

