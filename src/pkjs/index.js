var Clay = require('@rebble/clay');
var clayConfig = require('./config');

var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

Pebble.addEventListener('showConfiguration', function () {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function (e) {
  if (!e || !e.response) {
    return;
  }

  var settings = clay.getSettings(e.response);
  Pebble.sendAppMessage(
    settings,
    function () {
      console.log('Mood Ring Face settings sent');
    },
    function (error) {
      console.log('Mood Ring Face settings failed: ' + JSON.stringify(error));
    }
  );
});
