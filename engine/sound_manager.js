/* Copyright (C) 2012-2014 Carlos Pais
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

(function(SoundManager){
  
var channels = {
  "voice" : [],
  "music" : [],
  "sound" : []
};

var sounds =  {};

function _getSound(sound) {
  if (typeof sound == "string" && sound in sounds)
    return sounds[sound];
  else if (sound instanceof buzz.sound)
    return sound;
  return null;
}

function _getSounds(channel) {
  var sounds = null;
  if (channel)
      sounds = new buzz.group(channels[channel]);
  else
      sounds = buzz.all();
  return sounds;
}
  
function _addSound(sound, channel, options) {
  options = options || {};
  var src = sound;
  sound = _getSound(sound, channel);
  
  if (! sound && typeof src == "string") {
    sound = new buzz.sound(src);
    sounds[src] = sound;
    sound.src = src;
    if (channel)
      channels[channel] = sound;
  }
  
  if (sound instanceof buzz.sound) {
    sound.fadingOut = false;

    if (options.loop)
      sound.loop();
    else
      sound.unloop();
    if (belle.utils.isNumber(options.volume) && options.volume != sound.getVolume())
      sound.setVolume(options.volume);
    
    return sound;
  }
  
  return null;
}

function SoundManager()
{
}

SoundManager.prototype.play = function(sound, channel, options) {
  sound = _addSound(sound, channel, options);
  
  if (sound) {
    sound.play();
  }
}
  
SoundManager.prototype.stop = function(sound, channel, fade) {
  sound = _getSound(sound, channel);
  
  if (sound) {
    if (fade) {
      sound.fadeOut(fade, function() {
          sound.stop();
      });
    }
    else {
      sound.stop();
    }
  }
}

SoundManager.prototype.stopAll = function(channel, fade) {
  var sounds = _getSounds(channel);

  if (sounds)
      sounds.stop();
}

SoundManager.prototype.pause = function(sound, channel, fade) {
  sound = _getSound(sound);
  
  if (sound)
    sound.stop();
}

SoundManager.prototype.pauseAll = function(channel, fade) {
  var sounds = _getSounds(channel);

  if (sounds)
    sounds.pause();
}

SoundManager.prototype.mute = function(sound, channel) {
  sound = _getSound(sound, channel);
  
  if (sound)
    sound.mute();
}

SoundManager.prototype.muteAll = function(channel) {
  var sounds = _getSounds(channel);

  if (sounds)
    sounds.mute();
}

SoundManager.prototype.unmute = function(sound, channel) {
  sound = _getSound(sound, channel);
  
  if (sound)
    sound.unmute();
}

SoundManager.prototype.unmuteAll = function(channel) {
  var sounds = _getSounds(channel);

  if (sounds)
    sounds.unmute();
}

SoundManager.prototype.load = function(sound, channel, options) {
  _addSound(sound, channel, options);
}

SoundManager.prototype.getSounds = function(channel) {
  return _getSounds(channel).getSounds();
};

SoundManager.prototype.getPlayingSounds = function(channel) {
  var sounds = this.getSounds();
  var playing_sounds = [];
  for(var i=0; i < sounds.length; i++) {
      if (sounds[i].isPaused() === false)
        playing_sounds.push(sounds[i]);
  }
  return playing_sounds;
};
  
belle.SoundManager = SoundManager;

})(belle);
