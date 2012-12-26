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

namespace Game { namespace api { namespace sound {

    // Structs ----------------------------------------------------------------
    typedef struct {
        std::string filename;
        ALLEGRO_SAMPLE *sample;
        bool loaded;

    } Sound;

    typedef std::map<const std::string, Sound*> SoundMap;


    // Macros -----------------------------------------------------------------
    #define mapSound(a, m) \
        if (a.Length() > 0) { \
            m = getSound(ToString(a[0])); \
            if (!m->loaded) { \
                m = NULL; \
            } \
        } \
    

    // Loader -----------------------------------------------------------------
    SoundMap *sounds;
    Sound* getSound(std::string filename) {
        
        // Check if we need to load the sound
        SoundMap::iterator it = sounds->find(filename);
        if (it == sounds->end()) {
            
            Sound *sound = new Sound();
            sound->filename = filename;
            sound->sample = io::sample::open(filename);
            sound->loaded = sound->sample != NULL;
            sounds->insert(std::make_pair(filename, sound));

            return sound;

        } else {
            return it->second;
        }

    }


    // Sample Instances -------------------------------------------------------
    typedef std::vector<ALLEGRO_SAMPLE_INSTANCE*> SampleList;
    SampleList *instances;

    ALLEGRO_SAMPLE_INSTANCE *getInstanceForSample(Sound *sound) {

        // Find a available sample instance
        ALLEGRO_SAMPLE_INSTANCE *sampleInstance = NULL;
        for(SampleList::iterator it = instances->begin(); it != instances->end(); it++) {

            ALLEGRO_SAMPLE_INSTANCE *instance = *it;

            // If the instance is not attached it's available
            if (!al_get_sample_instance_attached(instance)) {

                debugArgs("api::sound", "Re-use sample instance for '%s'", sound->filename.data());
                sampleInstance = instance;
                al_set_sample(sampleInstance, sound->sample);
                al_set_sample_instance_gain(sampleInstance, 1.0f);
                al_set_sample_instance_pan(instance, 0.0f);
                al_set_sample_instance_speed(instance, 1.0f);
                al_attach_sample_instance_to_mixer(instance, al_get_default_mixer());
                break;

            }

        }

        // Create a new one if we're under the limit and no other is free
        if (sampleInstance == NULL) {

            debugArgs("api::sound", "Create sample instance for '%s'", sound->filename.data());
            sampleInstance = al_create_sample_instance(sound->sample);
            al_attach_sample_instance_to_mixer(sampleInstance, al_get_default_mixer());
            instances->push_back(sampleInstance);

        }
        
        return sampleInstance;

    }


    // API --------------------------------------------------------------------
    v8::Handle<v8::Value> load(const v8::Arguments& args) {

        Sound *sound;
        mapSound(args, sound);
        if (sound) {
            return v8::True();
        } 

        return v8::False();

    }

    v8::Handle<v8::Value> play(const v8::Arguments& args) {

        Sound *sound;
        mapSound(args, sound);

        if (sound && sound->loaded) {

            ALLEGRO_SAMPLE_INSTANCE *instance = getInstanceForSample(sound);
            if (instance) {

                debugArgs("api::sound", "Play sound '%s'", sound->filename.data()); // TODO log name, volume, pan, speed
                
                if (args.Length() > 1 && args[1]->IsNumber()) {
                    float gain = ToFloat(args[1]);
                    al_set_sample_instance_gain(instance, gain);
                }

                if (args.Length() > 2 && args[2]->IsNumber()) {
                    float pan = ToFloat(args[2]);
                    al_set_sample_instance_pan(instance, pan);
                }
                
                if (args.Length() > 3 && args[3]->IsNumber()) {
                    float speed = ToFloat(args[3]);
                    al_set_sample_instance_speed(instance, speed);
                }

                al_set_sample_instance_playing(instance, true);
                
                return v8::True();

            } else {
                return v8::False();
            }
            
        } else {
            return v8::False();
        }

    }


    // Export -----------------------------------------------------------------
    void init(v8::Handle<v8::Object> object) {

        sounds = new SoundMap();

        instances = new SampleList();
        setFunctionProp(object, "load", load);
        setFunctionProp(object, "play", play);

    }

    void update(double time, double dt) {
        
        for(SampleList::iterator it = instances->begin(); it != instances->end(); it++) {

            ALLEGRO_SAMPLE_INSTANCE *instance = *it;

            // Check for the instance having finished playing and detach it
            // this marks it as avaialble
            if (al_get_sample_instance_attached(instance)) {
                if (!al_get_sample_instance_playing(instance)) {
                    debugArgs("api::sound", "Done sample instance for '%s'", "unkown");
                    //printf("[api::sound] Sample instance finished playing\n");
                    al_detach_sample_instance(instance);
                }
            }

        }

    }

    void shutdown() {

        debugMsg("api::sound", "Shutdown...");

        for(SampleList::iterator it = instances->begin(); it != instances->end(); it++) {
            debugMsg("api::sound", "Destroyed sample instance");
            al_destroy_sample_instance(*it);
        }
        instances->clear();
        delete instances;

        for(SoundMap::iterator it = sounds->begin(); it != sounds->end(); it++) {
            Sound *snd = it->second;
            if (snd->sample) {
                debugArgs("api::sound::sample", "Destroyed '%s'", snd->filename.data());
                al_destroy_sample(snd->sample);
            }
            debugArgs("api::sound", "Destroyed '%s'", snd->filename.data());
            delete snd;
        }

        sounds->clear();
        delete sounds;

    }

}}}

