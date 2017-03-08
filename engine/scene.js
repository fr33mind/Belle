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

(function(belle) {

var Color = belle.graphics.Color,
    Frame = belle.Frame,
    Object = belle.core.Object;
  
function Scene(data, parent)
{
    Frame.call(this, data, parent);

    this.objects = [];
    this.actions = [];
    this.action = null;
    this.image = null;
    this.finished = false;
    this.tries = 0;
    this.setVisible(false);
    this.load(data);
}

belle.extend(Scene, Frame);

Scene.prototype.load = function(data)
{
  Frame.prototype.load.call(this, data);

  this.setActive(false);  
  this.finished = false;
  this.loadObjects(data);
  this.loadActions(data);
  this.action = null;

  if (data) {
      if (data.index) {
	var action = this.getAction(data.index-1);
	if (action) {
	  this.setCurrentAction(data.index);
	  action.setFinished(true);
	}
      }
  }  
}

Scene.prototype.getObjects = function()
{
  return this.objects;
}

Scene.prototype.getActions = function()
{
  return this.actions;
}

Scene.prototype.reloadObjects = function(data) 
{
  if (!data || ! data.objects)
    return;
  
  var dataObjects = data.objects;
  for(var i=0; i < this.objects.length; i++) {
    this.objects[i].reload();
  }
}

Scene.prototype.reloadActions = function() 
{
  for(var i=0; i < this.actions.length; i++) {
    this.actions[i].reload();
  }
}

Scene.prototype.loadActions = function(data) 
{
  this.actions = [];
  
  //instanciate actions
  if (data.actions && data.actions.length > 0) {
      var actions = data.actions;
      for (var j=0; j < actions.length; j++) {
	  var action = belle.createAction(actions[j], this);
          if (action) {
            this.actions.push(action);
          }
      }
  }
}

Scene.prototype.loadObjects = function(data) 
{
  this.objects = [];

  //instanciate objects
  if (data.objects && data.objects.length > 0) {
    for(var j=0; j !== data.objects.length; j++) {
      var object = data.objects[j];
      obj = this.getGame().createObject(object, this);
      if (obj)
        this.objects.push(obj);
    }
  }
}

Scene.prototype.setVisible = function(visible) {
  if (this.visible == visible)
    return;

  Frame.prototype.setVisible.call(this, visible);

  if (visible) {
    //when transitions for scenes are added, call the animation here and set scene
    //to active after the animation is finished
    this.setActive(true);
  }
  else {
    this.setActive(false);
    this.setFinished(true);
    this.action = null;
  }
}

Scene.prototype.setActive = function(active) {
  this.active = active;
  if (active)
    this.trigger("activated");
  else
    this.trigger("deactivated");
}

Scene.prototype.isActive = function() {
  return this.active;
}

Scene.prototype.setFinished = function(finished) {
  this.finished = finished;
  if (finished)
    this.trigger("finished");
}

Scene.prototype.isFinished = function() {
  return this.finished;
}

Scene.prototype.addObject = function(object, temp) {
  if (object.visible)
    object.redraw = true;
  if (! temp)
    object.parent = this;
  this.objects.push(object);
  this.trigger("objectAdded", {object: object});
}

Scene.prototype.removeObject = function(object) {
    var index = this.indexOf(object);
    if (index != -1) {
      this.objects.splice(index, 1);
      this.trigger("objectRemoved", {object: object});
    }
}

Scene.prototype.goto = function(action) {
    this.setCurrentAction(action);
}

Scene.prototype.indexOf = function(object) {
    if (belle.isInstance(object, belle.objects.Object))
        return this.objects.indexOf(object);
    else if (belle.isInstance(object, belle.actions.Action))
        return this.actions.indexOf(object);
    return -1;
}

Scene.prototype.getActions = function() {
    return this.actions;
}

Scene.prototype.setCurrentAction = function(action) {
  var oldAction = this.action;
  var actions = this.getActions();
  var action = this.getAction(action);

  if (! action)
    return;

  this.action = action;
  
  if (oldAction != this.action) {
    if (oldAction)
      oldAction.setFinished(false);
  
    if (this.action)
      this.action.execute();
  }
}

Scene.prototype.getCurrentAction = function() {
  return this.getAction();
}

Scene.prototype.getAction = function(action)
{
    var actions = this.getActions();

    if (! action)
      return null;

    if (typeof action == "number") {
      if (action >= 0 && action < this.actions.length)
	return this.actions[action];
    }
    else if (typeof action == "string") {
        for (var i=0; i < this.actions.length; i++)
            if (this.actions[i].name == action)
                return this.actions[i];
    }
    else if (belle.isInstance(action, belle.actions.Action) && actions.indexOf(action) !== -1) {
      return action;
    }
    
    return null;    
}

Scene.prototype.getObject = function(name) {
    if (name) {
      for (var i=0; i < this.objects.length; i++)
	if (this.objects[i].name == name)
	  return this.objects[i];
    }
    
    return null;    
}

Scene.prototype.nextAction = function() {
  if (this.isFinished() || ! this.active)
    return;

  var index = this.actions.indexOf(this.action) + 1;
  if (index >= 0 && index < this.actions.length) {
    this.setCurrentAction(this.actions[index]);
  }
  else {
    this.setFinished(true);
  }
}

Scene.prototype.getNextAction = function(action) {
  var index = this.actions.indexOf(action) + 1;
  if (index >= 0 && index < this.actions.length) {
    return this.actions[index];
  }

  return null;
}

Scene.prototype.scale = function(widthFactor, heightFactor)
{
    this.setX(this.x);
    this.setY(this.y);
    this.setWidth(this.width);
    this.setHeight(this.height);      
}

Scene.prototype.serialize = function()
{
  var data = Frame.prototype.serialize.call(this);
  var objects = [];
  //only serialize objects belonging to the scene (not temporary)
  for(var i=0; i < this.objects.length; i++) {
    if (this.objects[i].parent == this)
      objects.push(this.objects[i]);
  }
  data["objects"] = belle.serialize(objects);
  return data;
}

belle.Scene = Scene;

}(belle));
