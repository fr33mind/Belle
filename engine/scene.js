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

var Color = belle.objects.Color;
  
function Scene(data)
{
    this.objects = [];
    this.actions = [];
    this.action = null;
    this.image = null;
    this.ready = false;
    this.finished = false;
    this.redrawBackground = false;
    this.name = data.name || "";
    this.backgroundImage = null;
    this.backgroundColor = null;
    this.backgroundImageLoaded = false;
    this.width = this.scaledWidth = game.width;
    this.height = this.scaledHeight = game.height;
    this.x = this.scaledX = 0;
    this.y = this.scaledY = 0;
    data.width = this.width;
    data.height = this.height;
    this.visible = true;
    this.tries = 0;
    this.eventListeners = {};
    this.states = [];
    
    this.element = document.createElement("div");
    this.backgroundElement = document.createElement("div");
    $(this.element).attr("id", data["name"]);
    $(this.element).addClass("scene");
    $(this.element).width(this.width);
    $(this.element).height(this.height);
    
    belle.utils.initElement(this.element, data);
    belle.utils.initElement(this.backgroundElement, data);
    this.backgroundElement.style.display = "block";

    this.load(data);
}

Scene.prototype.load = function(data)
{
  this.setActive(false);  
  this.finished = false;
  this.loadObjects(data);
  this.loadActions(data);
  this.action = null;

  if (data) {
      var backgroundImage, backgroundColor;
    
      if ("backgroundImage" in data)
	  backgroundImage = data["backgroundImage"];
      if ("backgroundColor" in data)
	  backgroundColor = new Color(data["backgroundColor"]);
      if ("name" in data)
	  this.name = data["name"];
      if (backgroundImage)
	this.setBackgroundImage(backgroundImage);
      if (backgroundColor)
	this.setBackgroundColor(backgroundColor);
      
      if (data.index) {
	var action = this.getAction(data.index-1);
	if (action) {
	  this.setCurrentAction(data.index);
	  action.setFinished(true);
	}
      }
  }  
}

Scene.prototype.reloadObjects = function(data) 
{
  if (!data || ! data.objects)
    return;
  
  var dataObjects = data.objects;
  for(var i=0; i < dataObjects.length; i++) {
    var obj = this.getObject(dataObjects[i].name);
    if (obj)
	obj.load(dataObjects[i]);
  }
}

Scene.prototype.reloadActions = function() 
{
  for(var i=0; i < this.actions.length; i++) {
    this.actions[i].setFinished(false);
  }
}

Scene.prototype.loadActions = function(data) 
{
  if (this.actions.length) {
    this.reloadActions();
    return;
  }
  
  //instanciate actions
  if (data.actions && data.actions.length > 0) {
      var actions = data.actions;
      for (var j=0; j < actions.length; j++) {
	  var action = belle.createAction(actions[j], this);
          if (action) {
            this.actions.push(action);
            this.states.push({});
          }
      }
  }
}

Scene.prototype.loadObjects = function(data) 
{
  if (this.objects.length) {
    this.reloadObjects(data);
    return;
  }
  //instanciate objects
  if (data.objects && data.objects.length > 0) {
      for(var j=0; j !== data.objects.length; j++) {
	  var object = data.objects[j];
	  obj = belle.createObject(object, this);
	  if (obj) 
	      this.objects.push(obj);
      }
  }
}

Scene.prototype.addEventListener = function(event, listener)
{
  var events = []; 
  if(this.eventListeners.hasOwnProperty(event))
    events =  this.eventListeners[event];
  events.push(listener);
  this.eventListeners[event] = events;
}

Scene.prototype.hide = function() {
  this.setActive(false);
  this.setFinished(true);
  this.action = null;
  belle.display.hideScene(this);
}

Scene.prototype.display = function() {
  belle.display.displayScene(this);
 
  //when transitions for scenes are added, call the animation here and set scene
  //to active after the animation is finished
  this.setActive(true);
}

Scene.prototype.setActive = function(active) {
  var listeners = [];
  if(active && this.eventListeners.hasOwnProperty("activated")) {
    listeners = this.eventListeners["activated"];
  }
  else if(! active && this.eventListeners.hasOwnProperty("deactivated")) {
    listeners = this.eventListeners["deactivated"];
  }

  for(var i=0; i < listeners.length; i++)
    listeners[i].call(this);
    
  this.active = active;
}

Scene.prototype.isActive = function() {
  return this.active;
}

Scene.prototype.setFinished = function(finished) {
  this.finished = finished;
}

Scene.prototype.isFinished = function() {
  return this.finished;
}

Scene.prototype.addObject = function(object) {
    this.objects.push(object);
    if (game.getScene() == this) {
        belle.display.addObject(object, this.element);
    }
}

Scene.prototype.removeObject = function(object) {
    var index = this.indexOf(object);
    if (index != -1) {
      this.objects.splice(index, 1);
      belle.display.removeObject(object);
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
  if (typeof action == "number") {
    if (action >= 0 && action < actions.length)
      this.action = actions[action];
  }
  else if (action instanceof String || typeof action === 'string') {
      for (var i=0; i !== actions.length; i++) {
	  if (actions[i].name === action) {
	      this.action = actions[i];
	      break;
	  }
      }
  }
  else if (belle.isInstance(action, belle.actions.Action) && this.indexOf(action) !== -1){
      this.action = action;
  }
  
  if (oldAction != this.action) {
    if (oldAction)
      oldAction.setFinished(false);
  
    if (this.action && this.action)  
      this.action.execute();
  }
}

Scene.prototype.getCurrentAction = function() {
  return this.getAction();
}

Scene.prototype.getAction = function(id, type) {
      
    if (typeof id == "number") {
      if (id >= 0 && id < this.actions.length)
	return this.actions[id];
    }
    else if (typeof id == "string") {
        for (var i=0; i < this.actions.length; i++)
            if (this.actions[i].name == id && (! type || type == this.actions[i].type))
                return this.actions[i];
    }
    else if (id === undefined)
        return this.action;
    
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

Scene.prototype.hasNextAction = function() {
  var index = this.actions.indexOf(this.action) + 1;
  if (index >= 0 && index < this.actions.length)
    return true;
  return false;
}

Scene.prototype.nextAction = function() {
    if (this.finished || ! this.active)
      return;
    
    var index = this.actions.indexOf(this.action) + 1;
    if (index >= 0 && index < this.actions.length) { 
	this.setCurrentAction(this.actions[index]);
    }
    else {
      this.finished = true;
    }
}

Scene.prototype.setBackgroundImage = function(background)
{
    var that = this;
    var AnimationImage = belle.objects.AnimationImage;
    
    if (background instanceof AnimationImage) {
        if (this.backgroundImage && background == this.backgroundImage)
            return;
        this.backgroundImage = background;
        that.backgroundImageLoaded = true; //assume for now
        if (this.backgroundElement) {
            this.backgroundElement.style.backgroundColor = "";
            if (this.backgroundElement.childNodes.length > 0)
                this.backgroundElement.removeChild(this.backgroundElement.childNodes[0]);
            this.backgroundElement.appendChild(this.backgroundImage.img);
            //this.backgroundElement.style.backgroundImage = "url('" + background.src + "')";
        }
    }
    else if (typeof background == "string" || typeof background == "object") {
        if (this.backgroundImage && typeof background == "string" && background == this.backgroundImage.img.src)
            return;
        this.backgroundImage = new AnimationImage(background, this);
        if (this.backgroundElement) {
            this.backgroundElement.style.backgroundColor = "";
            if (this.backgroundElement.childNodes.length > 0)
                this.backgroundElement.removeChild(this.backgroundElement.childNodes[0]);
            this.backgroundElement.appendChild(this.backgroundImage.img);
            //this.backgroundElement.style.backgroundImage = "url('" + background + "')";
        }
    }
   
    if (this.backgroundImage)
        this.redrawBackground = true;
}

Scene.prototype.setBackgroundColor = function(color)
{
    if (this.backgroundColor != color) {
        
        if (color instanceof Array)
            color = new Color(color);

        this.backgroundColor = color;
        
        if (this.backgroundColor) {
            this.redrawBackground = true;
            this.backgroundElement.style.backgroundColor = this.backgroundColor.toHex();
        }
    }
}

Scene.prototype.paint = function(context)
{    
    if (this.backgroundImage) {
        this.backgroundImage.paint(context, 0, 0, this.width, this.height);
    }
    else if (this.backgroundColor) {
        context.fillStyle  = this.backgroundColor.toString();
        context.fillRect(this.x, this.y, this.width, this.height);
    }
    
    this.redrawBackground = false;
}

Scene.prototype.isReady = function()
{
    if (this.backgroundImage) {
        return this.backgroundImage.isReady();
    }
    return true;
}

Scene.prototype.frameChanged = function()
{
    this.paint(belle.display.bgContext);
}

Scene.prototype.scale = function(widthFactor, heightFactor)
{
    this.setX(this.x);
    this.setY(this.y);
    this.setWidth(this.width);
    this.setHeight(this.height);      
}

Scene.prototype.setX = function(x)
{
    this.x = this.scaledX = x;
    var scale = belle.display.scaledWidthFactor;
    if (scale != 1)
        this.scaledX *= scale;
    this.element.style.left = this.scaledX + "px";
    this.backgroundElement.style.left = this.scaledX + "px";
}

Scene.prototype.setY = function(y)
{
    this.y = this.scaledY = y;
    var scale = belle.display.scaledHeightFactor;
    if (scale != 1)
        this.scaledY *= scale;
    this.element.style.top = this.scaledY + "px";
    this.backgroundElement.style.top = this.scaledY + "px";
}

Scene.prototype.setWidth = function(width)
{
    this.width = this.scaledWidth = width;
    var scale = belle.display.scaledWidthFactor;
    if (scale != 1)
        this.scaledWidth *= scale;
    this.element.style.width = this.scaledWidth + "px";
    this.backgroundElement.style.width = this.scaledWidth + "px";
}


Scene.prototype.setHeight = function(height)
{
    this.height = this.scaledHeight = height;
    var scale = belle.display.scaledHeightFactor;
    if (scale != 1)
        this.scaledHeight *= scale;
    this.element.style.height = this.scaledHeight + "px";
    this.backgroundElement.style.height = this.scaledHeight + "px";
}

Scene.prototype.serialize = function()
{
  var data = {};
  var serialize = belle.serialize;
  
  data["name"] = this.name;
  data["objects"] = serialize(this.objects);
  if (this.backgroundImage)
    data["backgroundImage"] = serialize(this.backgroundImage);
  if (this.backgroundColor)
    data["backgroundColor"] = serialize(this.backgroundColor);
  data["width"] = this.width;
  data["height"] = this.height;
  data["index"] = this.indexOf(this.getCurrentAction());
  
  return data;
}

belle.Scene = Scene;

}(belle));
