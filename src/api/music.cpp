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
#include <algorithm>

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
        bool looping;
        bool loaded;

        float gain;
        float pan;
        float speed;

        float gainFrom;
        float panFrom;
        float speedFrom;

        float gainTo;
        float panTo;
        float speedTo;

        float gainDuration;
        float panDuration;
        float speedDuration;

    } Music;

    typedef std::map<const std::string, Music*> MusicMap;
    typedef std::vector<Music*> MusicList;


    // Helper -----------------------------------------------------------------
    MusicMap *songs;
    MusicList *playing;

    Music* getMusic(std::string filename) {
       
        MusicMap::iterator it = songs->find(filename);
        Music *m;

        bool created = false;
        if (it == songs->end()) {
            
            m = new Music();
            m->filename = filename;
            m->stream = NULL;
            m->looping = false;
            m->loaded = false; 
            m->state = MUSIC_STATE_STOPPED;

            m->gain = 1.0f;
            m->pan = 0.0f;
            m->speed = 1.0f;

            m->gainFrom = 1.0f;
            m->panFrom = 0.0f;
            m->speedFrom = 1.0f;

            m->gainTo = 1.0f;
            m->panTo = 0.0f;
            m->speedTo = 1.0f;

            m->gainDuration = 0.0f;
            m->panDuration = 0.0f;
            m->speedDuration = 0.0f;

            songs->insert(std::make_pair(filename, m));

            created = true;

        } else {
            m = it->second;
        }

        // Load audio stream
        if (!m->stream && (m->loaded || created)) {

            m->stream = io::stream::open(filename);
            if (m->stream) {
                al_set_audio_stream_playing(m->stream, false);
                al_set_audio_stream_playmode(m->stream, m->looping ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);
                al_set_audio_stream_gain(m->stream, m->gain);
                al_set_audio_stream_pan(m->stream, m->pan);
                al_set_audio_stream_speed(m->stream, m->speed);
                m->loaded = true;

            } else {
                m->loaded = false;
            }

        }

        return m;

    }

    Music *musicFromArg(const v8::Arguments& args) {

        Music *m = NULL;
        if (args.Length() > 0) {

            m = getMusic(ToString(args[0]));
            if (!m->loaded) {
                m = NULL;
            }

        }

        return m;

    }

    void playStream(Music *m) {

        debugArgs("music", "Play %s", m->filename.data());

        if (m->stream && !al_get_audio_stream_attached(m->stream)) {
            debugArgs("music", "Attach %s", m->filename.data());
            al_attach_audio_stream_to_mixer(m->stream, al_get_default_mixer());
        }

        playing->push_back(m);
        al_set_audio_stream_playing(m->stream, true);

    }

    void stopStream(Music *m) {

        debugArgs("music", "Stop %s", m->filename.data());

        playing->erase(std::remove(playing->begin(), playing->end(), m), playing->end());
        al_set_audio_stream_playing(m->stream, false);
        al_rewind_audio_stream(m->stream);

        if (m->stream && al_get_audio_stream_attached(m->stream)) {
            debugArgs("music", "Detach %s", m->filename.data());
            al_detach_audio_stream(m->stream);
        }

        al_destroy_audio_stream(m->stream);
        m->stream = NULL;

    }

    v8::Handle<v8::Value> setValue(const v8::Arguments& args, const Music *m, 
                                   float *value, float minLimit, float maxLimit,
                                   float *duration, float *from, float *to,
                                   bool (*setter)(ALLEGRO_AUDIO_STREAM*, float)) {

        if (m && args.Length() > 1 && args[1]->IsNumber()) {

            float val = ToFloat(args[1]);
            if (val >= minLimit && val <= maxLimit) {
                
                if (args.Length() > 2 && args[2]->IsNumber()) {

                    float d = ToFloat(args[2]);
                    if (d >= 0) {
                        *duration = ToFloat(args[2]);
                        *from = *value;
                        *to = val;
                        return v8::True();
                    }
            
                } else {
                    *from = val;
                    *to = val;
                    *value = val;
                    setter(m->stream, val);
                    return v8::True();
                }
            
            }
    
        }

        return v8::False();

    }

    void updateValue(ALLEGRO_AUDIO_STREAM *stream, float *value, 
                      const double dt, const float duration,
                      const float from, const float to,
                      bool (*setter)(ALLEGRO_AUDIO_STREAM*, float)) {
        
        if (*value != to) {
            float d = (to - from) / duration;
            *value = to > from ? std::min((*value) + d * dt, (double)to)
                               : std::max((*value) + d * dt, (double)to);
            
            setter(stream, *value);

        }

    }

    // Macros -----------------------------------------------------------------
    #define musicState(cmp, newState, action) \
        Music *m = musicFromArg(args); \
        if (m) { \
            if (m->state cmp) { \
                m->state = newState; \
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


    // API --------------------------------------------------------------------
    v8::Handle<v8::Value> load(const v8::Arguments& args) {
        return musicFromArg(args) ? v8::True() : v8::False();
    }

    v8::Handle<v8::Value> play(const v8::Arguments& args) {
        musicState(!= MUSIC_STATE_PLAYING, MUSIC_STATE_PLAYING, playStream(m));
    }

    v8::Handle<v8::Value> pause(const v8::Arguments& args) {
        musicState(== MUSIC_STATE_PLAYING, MUSIC_STATE_PAUSED, 
            debugArgs("music", "Pause %s", m->filename.data());
            playing->erase(std::remove(playing->begin(), playing->end(), m), playing->end());
            al_set_audio_stream_playing(m->stream, false);
        )
    }

    v8::Handle<v8::Value> resume(const v8::Arguments& args) {
        musicState(== MUSIC_STATE_PAUSED, MUSIC_STATE_PLAYING, 
            debugArgs("music", "Resume %s", m->filename.data());
            playing->push_back(m);
            al_set_audio_stream_playing(m->stream, true);
        )
    }

    v8::Handle<v8::Value> stop(const v8::Arguments& args) {
        musicState(!= MUSIC_STATE_STOPPED, MUSIC_STATE_STOPPED, stopStream(m));
    }

    v8::Handle<v8::Value> setVolume(const v8::Arguments& args) {
        Music *m = musicFromArg(args);
        return setValue(args, m, &m->gain, 0.0f, 1.0f, &m->gainDuration, 
                        &m->gainFrom, &m->gainTo, al_set_audio_stream_gain);
    }

    v8::Handle<v8::Value> setPan(const v8::Arguments& args) {
        Music *m = musicFromArg(args);
        return setValue(args, m, &m->pan, -1.0f, 1.0f, &m->panDuration, 
                        &m->panFrom, &m->panTo, al_set_audio_stream_pan);
    }

    v8::Handle<v8::Value> setSpeed(const v8::Arguments& args) {
        Music *m = musicFromArg(args);
        return setValue(args, m, &m->speed, 0.0f, 1.0f, &m->speedDuration, 
                        &m->speedFrom, &m->speedTo, al_set_audio_stream_speed);
    }

    v8::Handle<v8::Value> setLooping(const v8::Arguments& args) {
        Music *m = musicFromArg(args);
        if (m && args.Length() > 1) {

            bool loop = ToBoolean(args[1]);
            if (loop != m->looping) {
                al_set_audio_stream_playmode(m->stream, loop ? ALLEGRO_PLAYMODE_LOOP
                                                             : ALLEGRO_PLAYMODE_ONCE);
                m->looping = loop;
                return v8::True();
            }
            
        }

        return v8::False();
    }


    // Export -----------------------------------------------------------------
    void init(v8::Handle<v8::Object> object) {

        songs = new MusicMap();
        playing = new MusicList();
        setFunctionProp(object, "load", load);
        setFunctionProp(object, "play", play);
        setFunctionProp(object, "pause", pause);
        setFunctionProp(object, "resume", resume);
        setFunctionProp(object, "stop", stop);
        setFunctionProp(object, "setLooping", setLooping);
        setFunctionProp(object, "setVolume", setVolume);
        setFunctionProp(object, "setPan", setPan);
        setFunctionProp(object, "setSpeed", setSpeed);

    }

    void update(double time, double dt) {

        for(MusicList::iterator it = playing->begin(); it != playing->end(); it++) {

            Music *m = *it;
            if (m->stream && !al_get_audio_stream_playing(m->stream)) {

                debugArgs("api::music", "Stream '%s' ended", m->filename.data());
                stopStream(m); // Invalidates the iterator, thus, we need to break out
                m->state = MUSIC_STATE_STOPPED;
                break;

            }

        }

        for(MusicList::iterator it = playing->begin(); it != playing->end(); it++) {

            Music *m = *it;
            if (m->stream && m->state == MUSIC_STATE_PLAYING) {
                updateValue(m->stream, &m->gain, dt, m->gainDuration, m->gainFrom, m->gainTo, al_set_audio_stream_gain);
                updateValue(m->stream, &m->pan, dt, m->panDuration, m->panFrom, m->panTo, al_set_audio_stream_pan);
                updateValue(m->stream, &m->speed, dt, m->speedDuration, m->speedFrom, m->speedTo, al_set_audio_stream_speed);
            }

        }

    }

    void shutdown() {

        debugMsg("api::music", "Shutdown...");

        for(MusicMap::iterator it = songs->begin(); it != songs->end(); it++) {

            Music *song = it->second;
            if (song->stream) {
                debugArgs("api::music::stream", "Destroyed '%s'", song->filename.data());
                stopStream(song);
            }

            debugArgs("api::music", "Destroyed '%s'", song->filename.data());
            delete song;

        }

        songs->clear();
        delete songs;

        playing->clear();
        delete playing;

    }

}}}

