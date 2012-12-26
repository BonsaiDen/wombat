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

namespace Game { namespace api { namespace music {

    // Structs ----------------------------------------------------------------
    typedef enum MUSIC_STATE {
        MUSIC_STATE_STOPPED = 0,
        MUSIC_STATE_PLAYING = 1,
        MUSIC_STATE_PAUSED = 2
        
    } MUSIC_STATE;

    typedef struct {
        std::string filename;
        ALLEGRO_AUDIO_STREAM *stream;
        MUSIC_STATE state;
        bool loaded;
        bool looping;

    } Music;

    typedef std::map<const std::string, Music*> MusicMap;


    //// Macros -----------------------------------------------------------------
    #define mapMusic(a, m) \
        if (a.Length() > 0) { \
            m = getMusic(ToString(a[0])); \
            if (!m->loaded) { \
                m = NULL; \
            } \
        } \
    
    #define musicState(cmp, newState, action) \
        mapMusic(args, song); \
        if (song) { \
            if (cmp) { \
                song->state = newState; \
                action; \
                return v8::True(); \
            \
            } else { \
                return v8::False(); \
            } \
            \
        } else { \
            return v8::Undefined(); \
        }


    //// Loader -----------------------------------------------------------------
    MusicMap *songs;
    Music* getMusic(std::string filename) {
       
        MusicMap::iterator it = songs->find(filename);
        Music *song;

        bool created = false;
        if (it == songs->end()) {
            
            song = new Music();
            song->filename = filename;
            song->stream = NULL;
            song->looping = false;
            song->state = MUSIC_STATE_STOPPED;
            song->loaded = false; 
            songs->insert(std::make_pair(filename, song));

            created = true;

        } else {
            song = it->second;
        }

        // Load audio stream
        if (!song->stream && (song->loaded || created)) {

            song->stream = io::stream::open(filename);
            if (song->stream) {
                al_set_audio_stream_playing(song->stream, false);
                song->loaded = true;

            } else {
                song->loaded = false;
            }

        }

        return song;

    }

    void attachMusicStream(Music *music) {
       if (music->stream && !al_get_audio_stream_attached(music->stream)) {
           debugArgs("music", "Attach %s", music->filename.data());
           al_attach_audio_stream_to_mixer(music->stream, al_get_default_mixer());
       }
    }

    void detachMusicStream(Music *music) {
       if (music->stream && al_get_audio_stream_attached(music->stream)) {
           debugArgs("music", "Detach %s", music->filename.data());
           al_detach_audio_stream(music->stream);
       }
    }


    // API --------------------------------------------------------------------
    v8::Handle<v8::Value> load(const v8::Arguments& args) {

       Music *song;
       mapMusic(args, song);
       if (song) {
           return v8::True();
       } 

       return v8::False();


    }
    v8::Handle<v8::Value> play(const v8::Arguments& args) {

        Music *song;
        musicState(
            song->state != MUSIC_STATE_PLAYING,
            MUSIC_STATE_PLAYING, 
            attachMusicStream(song);
            debugArgs("music", "Play %s", song->filename.data());
            al_set_audio_stream_playing(song->stream, true);
        )
       
    }

    v8::Handle<v8::Value> pause(const v8::Arguments& args) {

        Music *song;
        musicState(
            song->state == MUSIC_STATE_PLAYING,
            MUSIC_STATE_PAUSED, 

            debugArgs("music", "Pause %s", song->filename.data());
            al_set_audio_stream_playing(song->stream, false);
        )

    }

    v8::Handle<v8::Value> resume(const v8::Arguments& args) {

        Music *song;
        musicState(
            song->state == MUSIC_STATE_PAUSED,
            MUSIC_STATE_PLAYING, 

            debugArgs("music", "Resume %s", song->filename.data());
            al_set_audio_stream_playing(song->stream, true);
        )

    }

    v8::Handle<v8::Value> stop(const v8::Arguments& args) {
       
        Music *song;
        musicState(
            song->state != MUSIC_STATE_STOPPED,
            MUSIC_STATE_STOPPED,

            debugArgs("music", "Stop %s", song->filename.data());
            al_set_audio_stream_playing(song->stream, false);
            al_rewind_audio_stream(song->stream);
            detachMusicStream(song);
            al_destroy_audio_stream(song->stream);
            song->stream = NULL;
        )

    }

    v8::Handle<v8::Value> setLooping(const v8::Arguments& args) {

        Music *song;
        mapMusic(args, song);
        if (song && args.Length() > 1) {

            bool loop = ToBoolean(args[1]);
            if (loop != song->looping) {
                al_set_audio_stream_playmode(song->stream, loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);
                song->looping = loop;
                return v8::True();
                
            } else {
                return v8::False();
            }
            
        } else {
            return v8::Undefined();    
        }

    }

    v8::Handle<v8::Value> setPan(const v8::Arguments& args) {
        return v8::Undefined();
    }

    v8::Handle<v8::Value> setVolume(const v8::Arguments& args) {
        return v8::Undefined();
    }


    // Export -----------------------------------------------------------------
    void init(v8::Handle<v8::Object> object) {

        songs = new MusicMap();
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

        for(MusicMap::iterator it = songs->begin(); it != songs->end(); it++) {

            Music *song = it->second;
            if (song->stream) {
                debugArgs("api::music::stream", "Destroyed '%s'", song->filename.data());
                al_set_audio_stream_playing(song->stream, false);
                al_rewind_audio_stream(song->stream);
                detachMusicStream(song);
                al_destroy_audio_stream(song->stream);
                song->stream = NULL;
            }

            debugArgs("api::music", "Destroyed '%s'", song->filename.data());
            delete song;

        }


        songs->clear();
        delete songs;
    }

}}}

