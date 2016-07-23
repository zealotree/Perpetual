var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

Pebble.addEventListener('ready', function() {
  // Update s_js_ready on watch
  Pebble.sendAppMessage({'AppKeyJSReady': 1});
  console.log("Ready received!");

});