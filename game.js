/*global game, keyboard, sound, music, graphics */
game.init = function(config) {
    config.title = 'Game';
    config.width = 256;
    config.height = 144;
    config.scale = 3;
    config.fps = 60;
};

game.load = function() {
    // TODO pass argv here
    console.log('Loading...');
    sound.load('effect.wav');
    music.load('music.ogg');
};

game.update = function(time, dt) {

    if (keyboard.wasPressed(keyboard.KEY_E)) {
        console.log('play music');
        music.play('music.ogg');
    }

    if (keyboard.wasPressed(keyboard.KEY_R)) {
        console.log('stop music');
        music.stop('music.ogg');
    }

    if (keyboard.wasPressed(keyboard.KEY_L)) {
        console.log('set music volume');
        music.setVolume('music.ogg', 0.5);
    }

    if (keyboard.wasPressed(keyboard.KEY_B)) {
        console.log('pause music');
        music.pause('music.ogg');
    }

    if (keyboard.wasPressed(keyboard.KEY_N)) {
        console.log('resume music');
        music.resume('music.ogg');
    }

    if (keyboard.wasPressed(keyboard.KEY_P)) {

        console.log('play sound');
        sound.play('effect.wav', 0.5, -1);
        game.sound = true;
    }

    if (keyboard.wasPressed(keyboard.KEY_M)) {
        console.log('play sound');
        sound.play('effect.wav', 1, 0, 2);
        game.sound = true;
    }

    if (keyboard.wasPressed(keyboard.F5)) {
        game.reload();
    }

    if (keyboard.wasPressed(keyboard.ESCAPE)) {
        game.quit();
    }

};

game.render = function(time) {

    if (game.sound) {
        graphics.setColor(255, 255, 255);
        graphics.rect(0, 0, 200, 200, true);
        game.sound = false;
    }


    graphics.setColor(255, 0, 0);
    graphics.rect(0, 0, game.config.width, game.config.height);
    graphics.setColor(255, 255, 0);
    graphics.rect(1, 1, game.config.width - 2, game.config.height - 2);

    graphics.setColor(0, 255, 255);
    graphics.rect(4, 4, game.config.width - 8, game.config.height - 8);

    graphics.setColor(0, 0, 255);
    graphics.rect(5, 5, game.config.width - 10, game.config.height - 10, true);

    graphics.setColor(255, 128, 0, Math.abs(Math.cos(time)));

    var y = Math.sin(time * 3) * game.config.height * 0.4;
    graphics.rect(game.config.width / 2 - 8, y + game.config.height / 2 - 8, 16, 16, true);

};

// TODO clean up JS struct
// modules
// templates
