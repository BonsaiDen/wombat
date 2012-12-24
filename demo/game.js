/*global game, keyboard, mouse, graphics, image */
var foo = require('foo');

game.init = function(config) {
    config.title = 'Kuusi';
    config.width = 320;
    config.height = 240;
    config.scale = 2;
    config.fps = 30;
};

game.load = function(arg) {
    image.load('player.png', 8, 8);
};

game.update = function(time, dt) {

    if (keyboard.isDown(keyboard.KEY_F)) {
        console.log('pressed');
    }

    if (keyboard.hasFocus()) {
        graphics.setBackColor(255, 0, 255);

    } else {
        graphics.setBackColor(255, 0, 0);
    }

    if (keyboard.wasPressed(keyboard.F5)) {
        game.reload();
    }

};

game.render = function(time) {
    graphics.rect(10, 10, 200, 200, true);
    image.drawTiled('player.png', 10, 10, 0);
};

