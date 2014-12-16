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
 
(function(game) {
    
	function getSceneData(scene) {
	  if (this.data && this.data.scenes && scene in this.data.scenes)
	    return this.data.scenes.scene;
	  return {};
	}
	
	game.addEventListener = function(event, obj, callback) {
	  if (typeof callback != "function" || ! event)
	    return;
	  
	  obj = obj || null;
	  var listeners = this.eventListeners[event] || [];
	  
	  var _callback = function() {
	    callback.call(obj);
	  }
	  
	  listeners.push(_callback);
	  this.eventListeners[event] = listeners;
	}
	
	game.triggerEvent = function(event) {
	  var listeners = this.eventListeners[event] || [];
	  for(var i=0; i < listeners.length; i++)
	    listeners[i].call(this);
	}
	
    	game.hasPauseScreen = function() {
	  if(this.pauseScreen.scenes.length)
	    return true;
	  return false;
	}
  
	game.setFinished = function(finished) {
	    this.finished = finished;
            if (finished)
              this.soundManager.stopAll();
	}
	
  	game.isFinished = function() {
	  return this.finished;
	}
	
	game.getSize = function() {
	  return this.getScenes().length;
	}
  
	game.indexOf = function(scene) {
	  if (scene && belle.isInstance(scene, belle.Scene)) {
	    var scenes = this.getScenes() || [];
	    return scenes.indexOf(scene);
	  }
	  return -1;
	}
	
	game.goto = function(scene) {
	  this.setCurrentScene(scene);
	}

	game.setCurrentScene = function(scene, data) {	     
	    var scene = this.getScene(scene);
	    if (scene) {  
	      //hide current scene
	      var currScene = this.getCurrentScene();
	      if (currScene && currScene != scene) {
		currScene.hide();
	      }
	      
	      if (data === undefined)
		data = getSceneData(scene.name)
	      
	      scene.load(data);
	      
	      if (this.paused)
		this.pauseScreen.scene = scene;
	      else
		this.scene = scene;

	      scene.display();
	    }
	}
	
	game.getScenes = function() {
		if (this.paused)
		  return this.pauseScreen.scenes;
		return this.scenes;
	}
	
	game.getCurrentScene = function() {
	  return this.getScene();
	}
	
	game.getScene = function(scene) {
	    if (scene === null || scene === undefined) {
	      if (this.paused)
		return this.pauseScreen.scene;
	      return this.scene;
	    }
	    
	    var scenes = this.getScenes();
	    if (typeof scene == "number") {
	      if (scene >= 0 && scene < scenes.length)
		return scenes[scene];
	    }
	    else if (scene instanceof String || typeof scene === 'string') {
		for (var i=0; i !== scenes.length; i++) {
		    if (scenes[i].name === scene) {
			return scenes[i];
		    }
		}
	    }
	    else if (belle.isInstance(scene, belle.Scene) && scenes.indexOf(scene) !== -1){
		return scene;
	    }
	    
	    return null;
	}
	
	game.hasNextScene = function() {
	    var index = this.indexOf(this.getScene()) + 1;
	    if (index >= 0 && index < this.getScenes().length)
	      return true;
	    return false;
	}
	
	game.nextScene = function() {
	  if (this.isFinished())
	    return;
	  
	  var index = this.indexOf(this.getScene()) + 1;
	  var scene = this.getScene(index);
	  if (scene) {
	    this.setCurrentScene(scene);
	  }
	  else {
	    this.setFinished(true);
	  }
	}
	
	game.next = function() {
		var action = null;
		while (! action && this.scene) {
			action = this.scene.nextAction();
			if (this.scene.isFinished())
				this.scene = this.nextScene();
		}
			
		if (! action)
			this.setFinished(true);
		return action;
	}
	
	game.getResource = function(name) {
		if (name && name in game.resources)
			return game.resources[name];
		return null;
	}
	
	game.getValue = function(variable) {
		if (! variable)
		  return "";
	  
		if (variable[0] == "$")
		  variable = variable.slice(1);
		
		return this.variables[variable] || "";
	}

	game.hasVariable = function(variable) {	
		return !! (this.getValue(variable));
	}

	game.addVariable = function (variable, value) {
	    if (this.getValue(variable) != value) {
	      this.variables[variable] = value;
	      this.triggerEvent("variableChanged");
	    }
	}
	
	game.isPaused = function() {
	  return this.paused;
	}
	
	game.pause = function() {
	  if (this.paused || ! this.hasPauseScreen())
	    return;

	  belle.display.showPauseScreen();
	  var scene = game.pauseScreen.scenes[0];
	  this.pauseScreen.scene = scene;
	  scene.action = null;
	  scene.display();
	  this.paused = true;
	}
	
	game.resume = function() {
	  if (this.paused) {
	    belle.display.hidePauseScreen();
	    this.paused = false;
	  }
	}

	game.replaceVariables = function(text) {
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
		  values.push(this.getValue(variables[i]));
		}
		
		//replace variables with the values previously extracted
		for(var i=0; i != values.length; i++) {
		  text = text.replace(variables[i], values[i]);
		}
		
		return text;
	}
	
	game.serialize = function() {
	  var data = {};
	  data.scene = this.scene.serialize();
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
	
	game._load = function(data) {
	  if (!data || ! data.scene || ! data.scene.name) {
	    alert("Couldn't load game data");
	    return false;
	  }
	  
	  this.resume();
	  var scene = this.getScene(data.scene.name);
	  if (scene) {
	    this.setCurrentScene(scene, data.scene);
	  }
	  this.variables = data.variables || {};
          if (data.sounds) {
            var sounds = data.sounds;
            for(var i=0; i < sounds.length; i++)
              this.soundManager.play(sounds[i].src, "sound", sounds[i]);
          }
	  
	  return true;
	}
	
	game.save = function(id) {
	    if (! id)
	      id = 0;
	    
	    var scene = this.scene;
	    var name = scene.name;
	    var title = this.title;
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
	  
	    var entry = this.serialize();
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
	
	game.load = function(id) {
	    var title = this.title;
	    var savedGames = this.getSavedGames();
	    var entry = null;

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
		this._load(entry);
	    }
	    else
		alert('Game "'+ id +'" could not be loaded');      
	}
	
	game.getSavedGames =  function() {
	    var title = this.title;
	    var gameData = $.jStorage.get(title, {});
	    var savedGames = gameData["savedGames"] || [];
	    return savedGames;
	}
	
	game.playSound = function(sound, options) {
            this.soundManager.play(sound, "sound", options);
        }
        
        game.stopSound = function(sound, fade) {
            this.soundManager.stop(sound, "sound", fade);
        }
	
})(game);
