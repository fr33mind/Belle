/* Copyright (C) 2012-2015 Carlos Pais
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

(function(belle) {

  var CoreObject = belle.core.Object,
      GameModel = belle.GameModel;

  function Game(data, parent)
  {
    CoreObject.call(this, parent);
    this.mainModel = null;
    this.pauseModel = null;
    this.paused = true;
    this.variables = {};
    this.properties = {};
    this.resources = {};
    this.soundManager = new belle.SoundManager();
    this.setAssetManager(new belle.AssetManager(data.assets));

    if (data.data)
      this.load(data.data);
  }

  belle.extend(Game, CoreObject);

  Game.prototype.load = function(data) {
    if (! data)
      return false;

    this.data = data;

    for (var member in data ) {
        if (member == "resources" || member == "scenes" || member == "pauseScreen")
            continue;
        this.properties[member] = data[member];
    }

    if (this.properties["textSpeed"] < 0)
        this.properties["textSpeed"] = 0;
    else if (this.properties["textSpeed"] > 100)
        this.properties["textSpeed"] = 100;

    this.properties["textDelay"] = 1000 / this.properties["textSpeed"];
    document.title = "Belle - " + this.properties["title"];

    //init resources
    if (data["resources"]) {
      var resources = data["resources"];
      for (var name in resources) {
        obj = belle.createObject(resources[name], this);

          if (obj)
            this.resources[name] = obj;
      }
    }

    var sharedModelData = {
      resources: this.resources,
      assetManager: this.assetManager,
      soundManager: this.soundManager,
      properties: this.properties
    };

    if ("scenes" in data) {
        var mainModelData = $.extend({}, sharedModelData, {"scenes": data["scenes"]});

        this.mainModel = new GameModel(mainModelData, this);

        this.mainModel.bind("finished", this, function() {
          this.soundManager.stopAll();
          this.trigger("finished");
        });

        this.mainModel.bind("paused", this, function() {
          this.paused = true;
          this.trigger("paused");
        });

        this.mainModel.bind("resumed", this, function() {
          this.paused = false;
          this.trigger("resumed");
        });
    }

    if ("pauseScreen" in data && "scenes" in data["pauseScreen"]) {
        var pauseModelData = $.extend({}, sharedModelData, {"scenes": data["pauseScreen"]["scenes"]});
        this.pauseModel = new GameModel(pauseModelData, this);
    }
  }

  Game.prototype.resume = function(assetManager) {
    this.mainModel.resume();
  }

  Game.prototype.setAssetManager = function(assetManager) {
    this.assetManager = assetManager;
    this.assetManager.bind("loaded", this, function() {
      this.resume();
    }, true);
  }

  Game.prototype.isReady = function() {
    if (! this.assetManager)
      return false;
    return this.assetManager.isLoaded();
  }

  Game.prototype.getAssetManager = function() {
    return this.assetManager;
  }

  Game.prototype.getSoundManager = function() {
    return this.soundManager;
  }

  Game.prototype.getMainModel = function() {
    return this.mainModel;
  }

  Game.prototype.getPauseModel = function() {
   return this.pauseModel;
  }

  Game.prototype.setFinished = function(finished) {
    this.mainModel.setFinished(true);
  }

  Game.prototype.isFinished = function() {
    return this.mainModel.isFinished();
  }

  Game.prototype.getResource = function(name) {
    if (name && name in this.resources)
      return this.resources[name];
    return null;
  }

  Game.prototype.getProperty = function(name) {
    if (name && name in this.properties)
      return this.properties[name];
    return "";
  }

  Game.prototype.getModel = function() {
    if (this.isPaused())
      return this.pauseModel;
    return this.mainModel;
  }

  Game.prototype.isPaused = function() {
    return this.mainModel.isPaused();
  }

  Game.prototype.pause = function() {
    this.mainModel.pause();
    this.pauseModel.start();
  }

  Game.prototype.resume = function() {
    this.pauseModel.pause();
    this.mainModel.resume();
  }

  Game.prototype.setPaused = function(paused) {
    if (paused)
      this.pause();
    else
      this.resume();
  }

  Game.prototype.createObject = function(data, parent) {
    var game = this.parent;
    if ("resource" in data) {
      var name = data["name"];
      var resources = this.data["resources"],
          resourceData = {};

      if (data["resource"] in resources) {
        resourceData = resources[data["resource"]];
        data = $.extend({}, resourceData, data);
      }
    }

    return belle.createObject(data, parent);
  }

  Game.prototype.createAction = function(data, parent) {
    return belle.createAction(data, parent);
  }

  Game.prototype.getVariableValue = function(variable) {
    if (! variable)
      return "";

    if (variable[0] == "$")
      variable = variable.slice(1);

    return this.variables[variable] || "";
  }

  Game.prototype.hasVariable = function(variable) {
    return !! (this.getVariableValue(variable));
  }

  Game.prototype.addVariable = function (variable, value) {
    if (this.variables[variable] != value) {
      this.variables[variable] = value;
      this.trigger("variableChanged");
    }
  }

  Game.prototype.replaceVariables = function(text) {
    if (! text)
      return text;

    if (! text.contains("$"))
      return text;

    var validChar = /^[a-zA-Z_0-9]$/;
    var variable = "";
    var variables = [];
    var values = [];
    var appendToVariable = false;

    //Parse text to determine variables
    //Variables start from "$" until the end of string or
    //until any other character that is not a letter nor a digit.
    for(var i=0; i !== text.length; i++) {

      if (text.charAt(i).search(validChar) == -1) {
            appendToVariable = false;
            if (variable)
              variables.push(variable);
            variable = "";
            if(text.charAt(i) == "$")
              appendToVariable = true;
      }

      if (appendToVariable)
            variable += text.charAt(i);
    }

    if (variable)
      variables.push(variable);

    //replace variables with the respective values and append them to the values list
    for(var i=0; i != variables.length; i++) {
      values.push(this.getVariableValue(variables[i]));
    }

    //replace variables with the values previously extracted
    for(var i=0; i != values.length; i++) {
      text = text.replace(variables[i], values[i]);
    }

    return text;
  }

  Game.prototype.getState = function()
  {
    var data = this.mainModel.getState();
    data.variables = this.variables;
    var sounds = this.soundManager.getPlayingSounds();
    data.sounds = [];
    for(var i=0; i < sounds.length; i++) {
      var snd = sounds[i];
      var sound = {
        "src": snd.src,
        "volume": snd.volume,
        "loop": snd.sound.loop
      }
      data.sounds.push(sound);
    }

    return data;
  }

  Game.prototype.loadState = function(state)
  {
    if (!state || ! state.scene || ! state.scene.name) {
      return false;
    }

    this.variables = state.variables || {};
    if (state.sounds) {
      var sounds = state.sounds;
      for(var i=0; i < sounds.length; i++)
        this.soundManager.play(sounds[i].src, "sound", sounds[i]);
    }
    this.mainModel.loadState(state);

    return true;
  }

  Game.prototype.saveSlot = function(id) {
    if (! id)
      id = 0;

    var scene = this.mainModel.getScene();
    var name = scene.name;
    var title = this.properties.title;
    var gameData = $.jStorage.get(title, {});
    var savedGames = gameData["savedGames"] || [];
    var i = 0;

    //if string passed, search for an empty slot for the savegame
    if (typeof id == "string") {
        for(i=0; i < savedGames.length; i++) {
            if (savedGames[i] === null) {
                id = i;
                break;
            }
        }
        name = id;
        id = i;
    }

    var entry = this.getState();
    entry.date = belle.utils.getSaveDate();
    entry.name = name;

    if (id >= savedGames.length)
        for(var i=savedGames.length; i <= id; i++)
            savedGames.push(null);

    savedGames[id] = entry;
    gameData["savedGames"] = savedGames;

    $.jStorage.set(title, gameData, {TTL: 0});

    return {name: entry.name, date: entry.date};
  }

  Game.prototype.loadSlot = function(id) {
    var title = this.properties.title,
      savedGames = this.getSavedGames(),
      entry = null,
      ok = false;

    if (! savedGames) {
        alert("You don't have any saved games.");
        return;
    }

    if (typeof id == "number") {
        if (id >= 0 && id < savedGames.length)
            entry = savedGames[id];
    }
    else if (typeof id == "string") {
        for(var i=0; i < savedGames.length; i++) {
            var _entry = savedGames[i];
            if (_entry && _entry.name == id) {
              entry = _entry;
              break;
            }
        }
    }

    if (entry) {
      ok = this.loadState(entry);
      if (ok) {
        this.mainModel.resume();
      }
    }
    else
        alert('Game "'+ id +'" could not be loaded');
  }

  Game.prototype.getSavedGames =  function() {
      var title = this.properties.title;
      var gameData = $.jStorage.get(title, {});
      var savedGames = gameData["savedGames"] || [];
      return savedGames;
  }

  Game.prototype.playSound = function(sound, options) {
      this.soundManager.play(sound, "sound", options);
  }

  Game.prototype.stopSound = function(sound, fade) {
      this.soundManager.stop(sound, "sound", fade);
  }

  belle.Game = Game;

}(belle));
