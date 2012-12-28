A *very* simple game engine built around Allegro and V8.

## Build

You'll need cmake installed.

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./wombat ../game
```
> You'll need Allegro 5 and libv8 to build wombat.


## Scope

This is meant for creating simple 2D games which can - potentially - later on
be ported to a compatible Web Version of the Engine.


## TODO

- a `util.inspect` equivilant
- better stack traces
- make hot code reloading work better
- more graphic routines
- utility for changing the window name etc.
- storage abstraction


## API

> Note: Incomplete listing below.


### Console

- __undefined__ log(...)


### Game

- __object__ config

- __number__ getTime()
- __number__ getDelta()
- __boolean__ pause()
- __boolean__ resume()
- __boolean__ isPaused()
- __undefined__ reload()
- __boolean__ quit()


### Keyboard

- __boolean__ isDown(__number__ keyCode)
- __boolean__ wasPressed(__number__ keyCode)
- __boolean__ wasReleased(__number__ keyCode)
- __boolean__ isDown(__number__ keyCode)
- __boolean__ hasFocus()
- __number__ getCount()


### Mouse

- __boolean__ isDown(__number__ button)
- __boolean__ wasPressed(__number__ button)
- __boolean__ wasReleased(__number__ button)
- __boolean__ isDown(__number__ button)
- __boolean__ hasFocus()
- __number__ getCount()
- __object__ getPosition()


### Graphics

- __undefined__ clear([__number__ r, __number__ g, __number__ b, __number__ a])
- __undefined__ setAutoClear(__boolean__ enabled)
- __undefined__ render()


### Image

- __boolean__ load(__string__ image [, __number__ cols, __number__ rows])
- __undefined__ draw(__string__ image, __number__ x, __number__ y [, __bool__ flipHorizonal, __bool__ flipVertical, __number__ alpha])
- __undefined__ setTiled(__string__ image, __number__ cols, __number__ rows)
- __undefined__ drawTiled(__string__ image, __number__ x, __number__ y, __number__ tileIndex [, __bool__ flipHorizonal, __bool__ flipVertical, __number__ alpha])


### Sound

- __boolean__ load(__string__ sound)
- __boolean__ play(__string__ sound [, __number__ volume, __number__ pan, __number__ speed])


### Music

- __boolean__ load(__string__ music)
- __boolean__ play(__string__ music)
- __boolean__ pause(__string__ music)
- __boolean__ resume(__string__ music)
- __boolean__ stop(__string__ music)
- __boolean__ setVolume(__string__ music, __number__ volume)
- __boolean__ setPan(__string__ music, __number__ pan)
- __boolean__ setSpeed(__string__ music, __number__ speed)

