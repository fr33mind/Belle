/* Copyright (C) 2015 Carlos Pais
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

  var CoreObject = belle.core.Object;

  function GameModel(data, parent)
  {
    CoreObject.call(this, parent);

    this.scenes = [];
    this.paused = true;
    this.action = this._nextAction = null;
    this.scene = this._nextScene = null;
    this.queue = [];
    this.game = this.parent;
    this.data = null;
    
    this.load(data);
  }

  belle.extend(GameModel, CoreObject);

  GameModel.prototype.load = function(data){
    if (! data)
      return false;

    this.data = data;
    this._setGameProperties(data);

    var scene,
        SceneClass = belle["Scene"],
        scenes = data.scenes;

    for(var i=0; i < scenes.length; i++) {
      scene = scenes[i];

      if (belle[scene.type] != SceneClass) {
          belle.log("Invalid Scene type: " + scene.type);
          continue;
      }

      scene.width = this.properties.width;
      scene.height = this.properties.height;
      var sceneObject = new SceneClass(scene, this);
      this.scenes.push(sceneObject);
    }

    return true;
  }

  GameModel.prototype._setGameProperties = function(data) {
    this.resources = data.resources;
    this.assetManager = data.assetManager;
    this.soundManager = data.soundManager;
    this.properties = data.properties;
  }

  GameModel.prototype.getGame = function() {
    return this.game;
  }

  GameModel.prototype.getSceneData = function(scene) {
    if (this.data && this.data.scenes) {
      for(var i=0; i < this.data.scenes.length; i++)
        if (this.data.scenes[i].name == scene)
          return this.data.scenes[i];
    }
    return {};
  }

  GameModel.prototype.getProperty = function(name) {
    if (this.data && this.data.scenes && scene in this.data.scenes)
      return this.data.scenes.scene;
    return {};
  }

  GameModel.prototype.setFinished = function(finished) {
      this.finished = finished;
      if (finished) {
        this.trigger("finished");
      }
  }

  GameModel.prototype.isFinished = function() {
    return this.finished;
  }

  GameModel.prototype.getSize = function() {
    return this.getScenes().length;
  }

  GameModel.prototype.indexOf = function(scene) {
    if (scene && belle.isInstance(scene, belle.Scene)) {
      var scenes = this.getScenes() || [];
      return scenes.indexOf(scene);
    }
    return -1;
  }

  GameModel.prototype.setScene = function(scene, reload) {
    var scene = this.getScene(scene),
        currScene = this.getScene(),
        self = this;

    if (scene) {
      //hide current scene
      if (currScene && currScene != scene) {
        currScene.unbind("finished", this);
        currScene.hide();
      }

      if (reload || typeof reload == "undefined")
        scene.reload();
      scene.bind("finished", this, function(){
        this.nextScene();
      }, true);

      scene.bind("objectAdded", this, function(data){
        this.trigger("objectAdded", data);
      }, true);

      scene.bind("objectRemoved", this, function(data){
        this.trigger("objectRemoved", data);
      }, true);

      this.scene = scene;
      this.scene.show();
      this.trigger("sceneChanged");
      //TODO: In the future use event for when scene is ready
      // (in case it has some transition animation)
      this.nextAction();
    }
  }

  GameModel.prototype.getScenes = function() {
    return this.scenes;
  }

  GameModel.prototype.getScene = function(scene) {
      if (scene === null || scene === undefined)
        return this.scene;

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

  GameModel.prototype.setNextScene = function(scene) {
    this._nextScene = this.getScene(scene);
  }

  GameModel.prototype.getNextScene = function() {
    if (this._nextScene)
      return this._nextScene;
    var index = this.indexOf(this.getScene()) + 1;
    if (index >= 0 && index < this.scenes.length)
      return this.scenes[index];
    return null;
  }

  GameModel.prototype.nextScene = function() {
    if (this.isFinished())
      return;

    if (this.isPaused()) {
      this.queue.push(function(){
        this.nextScene();
      });
      return;
    }

    if (this.action && !this.action.isFinished()) {
      this.action.unbind("finished", this);
      this.action.setFinished(true);
    }

    var scene = this.getNextScene();
    this._nextScene = null;
    if (scene) {
      this.setScene(scene);
    }
    else {
      this.setFinished(true);
    }
  }

  GameModel.prototype.setNextAction = function(action) {
    this._nextAction = this.getAction(action);
  }

  GameModel.prototype.getNextAction = function() {
    if (this._nextAction)
      return this._nextAction;
    return this.getScene().getNextAction(this.action);
  }

  GameModel.prototype.nextAction = function() {
    if (this.isPaused()) {
      this.queue.push(function(){
        this.nextAction();
      });
      return;
    }

    if (this.action && !this.action.isFinished()) {
      this.action.unbind("finished", this);
      this.action.setFinished(true);
    }

    this.action = this.getNextAction();
    this._nextAction = null;

    if (! this.action) {
      this.nextScene();
      return;
    }

    this.action.bind("finished", this, function() {
      this.nextAction();
    }, true);
    this.action.execute();
  }

  GameModel.prototype.getAction = function(action) {
    if (! action)
      return this.action;
    if (this.getScene())
      return this.getScene().getAction(action);
    return null;
  }

  GameModel.prototype.isPaused = function() {
    return this.paused;
  }

  GameModel.prototype.pause = function() {
    if (! this.paused) {
      this.paused = true;
      this.trigger("paused");
    }
  }

  GameModel.prototype.resume = function() {
    if (this.paused) {
      this.paused = false;
      this.trigger("resumed");

      for(var i=0; i < this.queue.length; i++)
        this.queue[i].call(this);

      if (! this.queue.length) {
        if (! this.scene)
          this.nextScene();
        else if (!this.action || this.action.isFinished())
          this.nextAction();
      }

      this.queue.length = 0;
    }
  }

  GameModel.prototype.start = function() {
    this.setFinished(false);
    this.scene = this.action = null;
    this.resume();
  }

  GameModel.prototype.getState = function() {
    var data = {
      scene: null,
      index: -1
    };

    if (this.scene) {
      data.scene = this.scene.serialize();
      data.index = this.scene.indexOf(this.action);
    }

    return data;
  }

  GameModel.prototype.loadState = function(state) {
    var scene = this.getScene(state.scene.name);
    if (scene) {
      var data = $.extend({}, scene.data, state.scene);
      scene.load(data);
      this.action = null;
      this._nextAction = scene.getAction(state.index);
      this.setScene(scene, false);
      return true;
    }

    return false;
  }

  belle.GameModel = GameModel;

}(belle));
