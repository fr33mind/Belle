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
    this.fontLibrary = new belle.graphics.FontLibrary();
    this.setAssetManager(new belle.AssetManager(data.assets));
    this.controller = null;
    this.actionsDataStack = [];
    this.watchedObjects = [];

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

    if (this.properties["font"])
      this.properties["font"] = this.fontLibrary.createFont(this.properties["font"]);

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
    
    if (this.fontLibrary)
      this.fontLibrary.assetManager = assetManager;
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

  Game.prototype.removeVariable = function (variable) {
    if (!this.variables.hasOwnProperty(variable))
      return;

    delete this.variables[variable];
    this.trigger("variableChanged");
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
        "name": snd.name,
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
    this.soundManager.stopAll();
    if (state.sounds) {
      var sounds = state.sounds;
      for(var i=0; i < sounds.length; i++) {
        var sound = this.assetManager.loadAsset(sounds[i].name, "Audio");
        if (sound) {
          this.soundManager.play(sound, "sound", sounds[i]);
        }
      }
    }
    this.mainModel.loadState(state);

    return true;
  }

  Game.prototype.getSlot = function(id) {
    var savedGames = this.getSavedGames(),
        entry = null;

    if (! savedGames) {
        return entry;
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

    return entry;
  }

  Game.prototype.saveSlot = function(id, name, screenshot) {
    var title = this.properties.title;
    var gameData = $.jStorage.get(title, {});
    var savedGames = gameData["savedGames"] || [];
    var i = 0;

    if (typeof name != "string") {
      var scene = this.mainModel.getScene();
      name = scene.name;
    }

    if (!belle.isNumber(id)) {
        for(i=0; i < savedGames.length; i++) {
            if (savedGames[i] === null) {
                id = i;
                break;
            }
        }
        id = i;
    }

    if (id < 0)
      return null;

    if (savedGames[id]) {
      var ok = confirm("Are you sure you want to overwrite the saved game?");
      if (!ok)
        return null;
    }

    var entry = this.getState();
    entry.date = belle.utils.getSaveDate();
    entry.name = name;
    entry.thumbnail = screenshot ? this.takeScreenshot() : null;

    if (id >= savedGames.length)
        for(var i=savedGames.length; i <= id; i++)
            savedGames.push(null);

    savedGames[id] = entry;
    gameData["savedGames"] = savedGames;

    $.jStorage.set(title, gameData, {TTL: 0});

    return {name: entry.name, date: entry.date, thumbnail: entry.thumbnail};
  }

  Game.prototype.loadSlot = function(id) {
    var entry = this.getSlot(id);
    var loaded = false;

    if (entry) {
      var ok = confirm("Loading a saved game will discard current unsaved progress.\n Are you sure?");
      if (!ok)
        return false;

      loaded = this.loadState(entry);
      if (loaded) {
        this.mainModel.resume();
      }
      else {
        alert('Game slot "'+ id +'" could not be loaded');
      }
    }

    return loaded;
  }

  Game.prototype.takeScreenshot = function() {
    if (!this.controller)
      return "";

    var mainView = this.controller.views.main;
    var bgCanvas = mainView.bgcanvas;
    var canvas = mainView.canvas;
    var mergedCanvas = canvas.clone();
    var ctx = mergedCanvas[0].getContext('2d');
    ctx.drawImage(bgCanvas[0], 0, 0);
    ctx.drawImage(canvas[0], 0, 0);
    return mergedCanvas[0].toDataURL("image/jpeg");
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

  Game.prototype.getFontLibrary = function() {
    return this.fontLibrary;
  }

  Game.prototype.addWatchedObject = function(object) {
    if (!object || this.watchedObjects.indexOf(object) != -1)
      return;

    this.watchedObjects.push(object);
  }

  Game.prototype.removeWatchedObject = function(object, restore) {
    var index = this.watchedObjects.indexOf(object);
    if (index == -1)
      return;

    if (restore === undefined)
      restore = true;

    if (restore)
      this.restoreWatchedObject(object);

    this.watchedObjects.splice(index, 1);
  }

  Game.prototype.getWatchedObjectFromAction = function(action) {
    if (!action)
      return null;

    var object, actionGroup;
    for(var i=0; i < this.watchedObjects.length; i++) {
      object = this.watchedObjects[i];
      actionGroup = object.mouseMoveActionGroup;
      if (actionGroup) {
        if (actionGroup.actions.indexOf(action) != -1)
          return object;
        
        var parentAction = action.getTopParentAction();
        if (parentAction && actionGroup.actions.indexOf(parentAction) != -1)
          return object;
      }
    }

    return null;
  }

  Game.prototype.shouldUndoAction = function(action) {
    var object = this.getWatchedObjectFromAction(action);
    if (object)
      return true;
    return false;
  }

  Game.prototype.saveUndoAction = function(action, undo) {
    var object = this.getWatchedObjectFromAction(action);
    if (!object)
      return;

    var data = {
      'watchedObject': object,
      'action': action,
      'undo': undo
    };

    this.actionsDataStack.push(data);
  }

  Game.prototype.restoreWatchedObject = function(object) {
    if (!object)
      return;

    for(var i=this.actionsDataStack.length-1; i >= 0; i--) {
      var data = this.actionsDataStack[i];
      if (data.watchedObject != object)
        continue;

      if (typeof data.undo == "function") {
        if (!data.action.isRunning())
          data.undo();
        else {
          (function(game, action, undo) {
            var handler = function() {
              undo();
              action.unbind("stopped", game, handler);
            };
            action.bind("stopped", game, handler, true);
          })(this, data.action, data.undo);
        }
      }
      this.actionsDataStack.splice(i, 1);
    }
  }

  belle.Game = Game;

}(belle));
