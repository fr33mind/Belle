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

/*********** ACTION ***********/

(function(actions) {
    
var GameObject = belle.GameObject,
    Color = belle.graphics.Color,
    Point = belle.core.Point;
 
function Action(data, parent)
{
    GameObject.call(this, data, parent);

    this.finished = false;
    this.interval = null;
    this.wait = null;
    this.needsRedraw = true;
    this.skippable = true;
    this.waiting = false;
    this.type = "Action";
    this.valid = true;
    this.elapsedTime = 0;
    this.eventListeners = {};
    this.objectName = "";
    this.object = null;
    var game = this.getGame();
    
    if (data) {
        if ("object" in data) {
          var scene = this.getScene();
          if (typeof data["object"] == "string") {

              if (scene)
                this.object = scene.getObject(data["object"]);
              this.objectName = data["object"];
          }
          else if (typeof data["object"] == "object") {
            this.objectName = data["object"].name;
            this.object = game.createObject(data["object"], this);
          }
        }
            
        if ("skippable" in data)
            this.skippable = data["skippable"];
        
        if ("wait" in data) {
            this.setWait(new Wait(data["wait"]));
        }

        if ("name" in data)
            this.name = data["name"];
        
        if ("type" in data)
            this.type = data["type"];
    }
}

belle.extend(Action, GameObject);

Action.prototype.getInterval = function() {
    return this.interval;
}

Action.prototype.setInterval = function(interval) {
    this.interval = interval;
}

Action.prototype.isFinished = function() {
    return this.finished;
}

Action.prototype.setFinished = function(finished) {
    if (finished == this.finished)
      return;

    if (finished && this.wait && !this.wait.isFinished()) {
      this.wait.execute();
      this.waiting = true;
      return;
    }

    if (this.wait)
      this.wait.setFinished(finished);
    this.waiting = false;
    this.finished = finished;
    if (this.finished)
      this.trigger("finished");
}

Action.prototype.skip = function() 
{
    if (! this.skippable)
        return false;

    if (this.wait && this.waiting) {
      return this.wait.skip();
    }

    this.setFinished(true);
    return true;
}

Action.prototype.reload = function ()
{
    this.setFinished(false);
    if (this.interval) {
        clearInterval(this.interval);
        this.interval = null;
    }
    this.elapsedTime = 0;
    if (this.wait)
      this.wait.reload();

    this.object = null;
    this.object = this.getObject();
}

Action.prototype.reset = function ()
{
}

Action.prototype.execute = function() 
{
    this.setFinished(true);
}

Action.prototype.getObject = function()
{
  if (this.object)
    return this.object;
  else if (this.objectName) {
    var scene = this.getScene();
    if (scene)
      return scene.getObject(this.objectName);
  }
  return null;
}

Action.prototype.setWait = function(wait)
{
  this.wait = wait;
  this.wait.bind("finished", this, function() {
    this.setFinished(true);
  }, true);
}

/*********** FADE ACTION ***********/

function Fade(data, parent) 
{
    Action.call(this, data, parent);
    
    if ("fadeType" in data)
        this.fadeType = data["fadeType"];
    if ("duration" in data)
        this.duration = data["duration"];
    
    this.duration *= 1000;
    this.intervalDuration = this.duration / 255;
    this.timePassed = 0;
    this.prevTime = 0;
    //this.object.color.alpha = 0;
    this.target = 1;
    this.increment = 1;

    if (this.fadeType == "in") {
        this.target = 255;
        this.increment = 1;
    }
    else if (this.fadeType == "out") {
        this.target = 0;
        this.increment = -1;
    }
    else {
        error("PropertyError: Fade type '" + this.type + "' is not a valid type");
    }
}

belle.extend(Fade, Action);

Fade.prototype.execute = function () {
    var t = this;

    var object = this.getObject();
    if (! object) {
      this.setFinished(true);
      return;
    }

    this.prevTime = new Date().getTime();
    this.timePassed = 0;
     
    this.interval = setInterval(function() {t.fade();}, this.intervalDuration);        
    this.getObject().redraw = true;
}

Fade.prototype.fade = function () {
    var now = new Date().getTime();
    var passed = now - this.prevTime;
    this.timePassed += passed;
    this.prevTime = now;
    var object = this.getObject();
   
    if (this.timePassed >= this.duration) {
        object.setOpacity(this.target);
    }
    
    var opacity = object.getOpacity();
  
    if ((this.fadeType == "in" && opacity >= this.target) ||
       (this.fadeType == "out" && opacity <= this.target)) {
        clearInterval(this.interval);
        this.interval = null;
        this.setFinished(true);
        this.timePassed = 0;
        return;
    }
    
    passed = passed > this.intervalDuration ? passed : this.intervalDuration;
    var increment = this.increment;
    if (this.intervalDuration)
      increment = parseInt(passed * this.increment / this.intervalDuration) || 1; 
    
    if (this.fadeType == "in") {
      if (opacity < this.target)
          object.setOpacity(opacity + increment);
    }
    else {
      if (opacity > this.target)
          object.setOpacity(opacity + increment);
    }
      
    object.redraw = true;
}

Fade.prototype.skip = function () {
    var skipped = Action.prototype.skip.call(this);
    if (skipped) {
      clearInterval(this.interval);
      this.getObject().setOpacity(this.target);
    }
    return skipped;
}

Fade.prototype.reset = function () {
    Action.prototype.reset.call(this);
    var object = this.getObject();
    object.setOpacity(this.target);
    object.setBackgroundOpacity(this.bgTarget);
}

/*********** SLIDE ACTION ***********/

function Slide(data, parent)
{
    Action.call(this, data, parent);
    this.destX = data["destX"];
    this.destY = data["destY"];
    this.slideType = data["slideType"];
    this.startX = 0;
    this.startY = 0;
    this.duration = 0;
    this.timePassed = 0;
    this.intervalDuration = 0;
    this.prevTime = null;
    
    if ("duration" in data)
        this.duration = data["duration"];
    
    this.duration *= 1000;

    if (! belle.utils.isNumber(this.destX))
      this.destX += this.slideType;
    else
      this.destX = parseInt(this.destX);

    if (! belle.utils.isNumber(this.destY))
      this.destY += this.slideType;
    else
      this.destY = parseInt(this.destY);
}

belle.extend(Slide, Action);

Slide.prototype.execute = function ()
{
    var t = this;
    var object = this.getObject();

    if (! object) {
      this.setFinished(true);
      return;
    }

    var destX = parseInt(this.destX),
        destY = parseInt(this.destY);
    if (! belle.utils.isNumber(this.destX))

        belle.utils.isNumber(this.destX) ? parseInt(this.destX) : this.destX + this.slideType;

    this.startX = object.x;
    this.startY = object.y;
    this.endX = typeof this.destX == "string" ? object.mapXFromName(this.destX) : this.destX;
    this.endY = typeof this.destY == "string" ? object.mapYFromName(this.destY) : this.destY;
    this.endX = this.endX !== null ? this.endX : this.startX;
    this.endY = this.endY !== null ? this.endY : this.startY;
    this.timePassed = 0;
    this.incXPerMs = this.incYPerMs = 0;
    this.incX = this.incY = 0;

    if (this.duration) {
      this.incXPerMs = belle.utils.distance(this.startX, 1, this.endX, 1) / this.duration;
      this.incYPerMs = belle.utils.distance(1, this.endY, 1, this.startY) / this.duration;
    }

    if (this.endX < this.startX)
      this.incXPerMs *= -1;
    if (this.endY < this.startY)
      this.incYPerMs *= -1;

    if (this.endX != this.startX)
      this.incX = parseInt(this.incXPerMs * 10);

    if (this.endY != this.startY)
      this.incY = parseInt(this.incYPerMs * 10);

    this.prevTime = new Date;
    this.interval = setInterval(function() { t.slide(); }, 10);
}

Slide.prototype.slide = function ()
{
    var now = new Date;
    var passed = now - this.prevTime;
    this.prevTime = now;
    this.timePassed += passed;
    var object = this.getObject();
    var x = object.x, y = object.y;
    var incX = this.incX;
    var incY = this.incY;

    if (this.incXPerMs) {
      var xdif = parseInt((this.startX + this.timePassed * this.incXPerMs) - (x + incX));
      incX = Math.round(incX + xdif);
    }

    if (this.incYPerMs) {
      var ydif = parseInt((this.startY + this.timePassed * this.incYPerMs) - (y + incY));
      incY = Math.round(incY + ydif);
    }

    x += incX;
    y += incY;

    if (incX > 0 && object.x > this.endX || incX < 0 && object.x < this.endX)
        x = this.endX;
    if (incY > 0 && object.y > this.endY || incY < 0 && object.y < this.endY)
        y = this.endY;

    object.moveTo(x, y);

    if (x == this.endX && y == this.endY) {
        clearInterval(this.interval);
        this.setFinished(true);
    }
}

Slide.prototype.skip = function () {
    var skipped = Action.prototype.skip.call(this);
    if (skipped) {
      clearInterval(this.interval);
      var object = this.getObject();
      if (object) {
        object.setX(this.endX);
        object.setY(this.endY);
      }
    }
}

/*********** DIALOGUE ACTION ***********/

function Dialogue(data, parent)
{
    Action.call(this, data, parent);

    this.character = null;
    this.speakerName = "";
    this.text = "";
    this.index = 0;
    this.lines = [];
    this.rawText = "";
    this.append = false;
    
    if (data) {
        if ("character" in data) {
            this.speakerName = data["character"];
            this.character = this.getScene().getObject(data["character"]);
        }
        
        if ("text" in data) {
            this.text = data["text"];
        }

        if ("append" in data) {
            this.append = data["append"];
        }
    }

    this.rawText = this.text;
}

belle.extend(Dialogue, Action);

Dialogue.prototype.execute = function () {
    var t = this,
        game = this.getGame();
    this.index = 0;
    var object = this.getObject();
    if (! object) {
        this.setFinished(true);
        return;
    }
    
    object.activateDefaultTextColor();

    if (typeof object.setSpeakerName == "function")
      object.setSpeakerName(this.character ? this.character.name : this.speakerName);

    if (this.character) {
      object.setTextColor(this.character.textColor);

      if (typeof object.setNameColor == "function")
        object.setNameColor(this.character.nameColor);
    }

    if (! this.append)
      object.setText("");

    this.text = game.replaceVariables(this.text);
    this.interval = setInterval(function() { t.updateText(); }, game.getProperty("textDelay"));
}

Dialogue.prototype.updateText = function() {

    if (this.index == this.text.length) {
        clearInterval(this.interval);
        this.interval = null;
        this.setFinished(true);
        return;
    }

    this.getObject().appendText(this.text.charAt(this.index));
    this.index += 1;
}

Dialogue.prototype.skip = function () {
  var skipped = Action.prototype.skip.call(this);
  if (skipped) {
      clearInterval(this.interval);
      var object = this.getObject();
      object.appendText(this.text.slice(this.index));
      this.index = this.text.length;
      object.redraw = true;
  }
}

Dialogue.prototype.reset = function () 
{
    Action.prototype.reset.call(this);
    this.index = 0;
    var object = this.getObject();
    if (object)
        object.text = "";
    if (object && object.speakerName !== undefined)
        object.speakerName = "";
}

/*********** WAIT ACTION ***********/

function Wait(data, parent)
{
    Action.call(this, data, parent);
    
    if ( "time" in data)
        this.time = data["time"] * 1000;
    if ("waitType" in data) {
        this.waitType = data["waitType"];
        if (this.waitType == "MouseClick")
            this.skippable = true;
        else if (this.waitType == "Forever")
            this.skippable = false;
    }
    this.needsRedraw = false;
}

belle.extend(Wait, Action);

Wait.prototype.execute = function ()
{
    var t = this;
    
    if (isNaN(this.time) || this.time < 0)
      this.time = 0;
    
    if (this.waitType == "Timed") {
        setTimeout(function() { t.setFinished(true); }, this.time);
    }
}

/*********** CHANGE VISIBILITY Action ***********/

function ChangeVisibility(data, parent)
{
    Action.call(this, data, parent);
    this.transitions = [];
    this.duration = 0;
    this.show = null;
    
    if ("show" in data)
      this.show = data.show;

    if ("transitions" in data) {
        var transitions = data["transitions"];
        for(var i=0; i !== transitions.length; i++) {             
            var action = belle.createAction(transitions[i], this);
            if (action) {
              action.skippable = this.skippable;
              this.transitions.push(action);

              if (transitions[i].duration && transitions[i].duration > this.duration )
                  this.duration = transitions[i].duration;
            }
        }
    }
    
    this.duration *= 1000;
}

belle.extend(ChangeVisibility, Action);

ChangeVisibility.prototype.execute = function () 
{
    var object = this.getObject();
    
    if (! object || object.visible == this.show) {
      this.setFinished(true);
      return;
    }
    
    if (object instanceof belle.objects.Character && this.characterState)
      object.setState(this.characterState);
      
    this.initObjectForTransitions();
    if (this.show)
      object.show();
    
    var that = this;
    for (var i=0; i < this.transitions.length; i++) {
        this.transitions[i].execute();
    }
    
    if (this.transitions.length === 0) {
        object.setVisible(this.show);
        this.setFinished(true);
    }
    else
        this.interval = setTimeout(function(){ that.check(); }, this.duration);
}

ChangeVisibility.prototype.check = function () 
{
    var finish = true;
    var object = this.getObject();
    
    for (var i=0; i < this.transitions.length; i++) {
        if (! this.transitions[i].isFinished()) {
            finish = false;
            break;
        }
    }
    
    if (finish) {
      object.setVisible(this.show);
      this.setFinished(true);
    }
    else {
      var that = this;
      setTimeout(function(){ that.check(); }, 10);
    }
}

ChangeVisibility.prototype.skip = function ()
{
    clearTimeout(this.interval);
    for (var i=0; i !== this.transitions.length; i++)
        this.transitions[i].skip();

    this.check();
}

ChangeVisibility.prototype.reload = function ()
{
    Action.prototype.reload.call(this);

    for (var i=0; i !== this.transitions.length; i++)
      this.transitions[i].reload();
}

ChangeVisibility.prototype.initObjectForTransitions = function () 
{
    var object = this.getObject();
    for (var i=0; i < this.transitions.length; i++) {
        if (belle.isInstance(this.transitions[i], Fade)) {
          if (this.show && ! object.isVisible())
            object.setOpacity(0);
        }
    }
}
ChangeVisibility.prototype.setFinished = function(finished) {
      Action.prototype.setFinished.call(this, finished);
      for (var i=0; i !== this.transitions.length; i++)
        this.transitions[i].setFinished(finished);
}

/*********** Show Action ***********/

function Show(data, parent)
{
    data.show = true;
    ChangeVisibility.call(this, data, parent);
    this.characterState = "";
    if ("state" in data)
      this.characterState = data["state"];
}

belle.extend(Show, ChangeVisibility);

/*********** HIDE CHARACTER ACTION ***********/

function Hide(data, parent)
{
    data.show = false;
    ChangeVisibility.call(this, data, parent);
}

belle.extend(Hide, ChangeVisibility);


/************* Change Background *****************/

function ChangeBackground(data, parent)
{
    Action.call(this, data, parent);
    var game = this.getGame(),
    assetManager = game ? game.getAssetManager() : null;

    if (! assetManager)
      return;

    this.backgroundImage = null;
    this.backgroundColor = null;

    if ("backgroundImage" in data){
         this.backgroundImage = assetManager.loadAsset(data["backgroundImage"], "Image");
    }
    
    if ("backgroundColor" in data)
        this.backgroundColor = data["backgroundColor"];
}

belle.extend(ChangeBackground, Action);

ChangeBackground.prototype.execute = function () 
{
    this.reset();
    var scene = this.getScene();
    if (scene) {
        if (this.backgroundImage)
            scene.setBackgroundImage(this.backgroundImage);
        if (this.backgroundColor)
            scene.setBackgroundColor(this.backgroundColor);
    }
    
    this.setFinished(true);
}

/************* Change State *****************/

function ChangeState(data, parent)
{
    Action.call(this, data, parent);
    this.state = data["state"];
}

belle.extend(ChangeState, Action);

ChangeState.prototype.execute = function () 
{
    var object = this.getObject();
    if (object && typeof object.setState == "function")
      object.setState(this.state);
    
    this.setFinished(true);
}

/************* LABEL *****************/

function Label (data, parent)
{
    Action.call(this, data, parent);
}

belle.extend (Label, Action);

/************* Go TO LABEL *****************/

function GoToLabel(data, parent)
{
    Action.call(this, data, parent);
    if ("label" in data)
        this.label = data["label"];
    this.needsRedraw = false;
}

belle.extend(GoToLabel, Action);

GoToLabel.prototype.execute = function()
{
   this.reset();
   var gameModel = this.getGameModel();
   
   if (! gameModel) {
        this.setFinished(true);
        return;
   }
   
   if (this.label instanceof String || typeof this.label === 'string') {
      gameModel.setNextAction(this.label);
   }
   else if (belle.isInstance(this.label, Label)){
      gameModel.setNextAction(this.label.name);
   }

   this.setFinished(true);
}

GoToLabel.prototype.resetActions = function(from, to)
{
    var scene = game.getScene();
    if (! scene)
      return;

    var actions =  scene.getActions();
    for(var i=from; i < to && i < actions.length; i++) {
        actions[i].reset();
    }
}

/************* Go To Scene *****************/

function GoToScene(data, parent)
{
    Action.call(this, data, parent);
    
    this.TargetType = {
      "Name" : 0,
      "Position": 1
    }
    
    this.target = "";
    if ("target" in data && typeof data["target"] === "string")
        this.target = data["target"];
    
    this.targetType = this.TargetType.Name;
    if ("targetType" in data && parseInt(data["targetType"]) !== NaN)
        this.targetType = data["targetType"];
    
    this.needsRedraw = false;
}

belle.extend(GoToScene, Action);

GoToScene.prototype.goto = function(target)
{
   var gameModel = this.getGameModel();

   if (this.targetType == this.TargetType.Position) {
     target = target.toLowerCase();
     if(target == "next")
       gameModel.setNextScene(gameModel.indexOf(gameModel.getScene())+1);
     else if(target  == "previous")
       gameModel.setNextScene(gameModel.indexOf(gameModel.getScene())-1);
     else if (target == "first")
       gameModel.setNextScene(0);
     else if (target == "last")
       gameModel.setNextScene(gameModel.getSize()-1);
     else
       return;
   }
   else {
     gameModel.setNextScene(target);
   }

   gameModel.nextScene();
}

GoToScene.prototype.execute = function()
{
   this.reset();
   this.goto(this.target);
   this.setFinished(true);
}


/************* Branch *****************/

function Branch(data, parent)
{
    Action.call(this, data, parent);
    this.condition = "";
    this.trueActions = [];
    this.falseActions = [];
    this.actions = [];
    this.result = null;
    var branch = this;
    var action;
    var actions;
   
    if ( "trueActions" in data) {
        actions = data["trueActions"];
        for(var i=0; i < actions.length; i++) {
          action = belle.createAction(actions[i], this);
          if (action)
            this.trueActions.push(action);
        }
    }
    
    if ( "falseActions" in data) {
        actions = data["falseActions"];
        for(var i=0; i < actions.length; i++) {
          action = belle.createAction(actions[i], this);
          if (action)
            this.falseActions.push(action);
        }
    }
    
    actions = this.trueActions.concat(this.falseActions);
    for(var i=0; i < actions.length; i++) {
      actions[i].bind("finished", this, function(){
        this.actionFinished(this);
      });
    }
    
    if ("condition" in data)
      this.loadCondition(data["condition"]);
    
    /*if ("condition" in data) {
        var condition = data["condition"];
        var parts = condition.split(" ");
        var keywords = ["in", "true", "false", "defined", "undefined", "and", "or"];
        var operators = ["==", "!=" , ">" , ">=", "<", "<="];
        
        for(var i=0; i < parts.length; i++) {
            if (keywords.indexOf(parts[i]) !== -1 || parseInt(parts[i]) !== NaN || parts[i].indexOf('"') != -1)
                continue;
            parts[i] = "Novel.variables['" + parts[i] + "']";
        }
        
        if (condition.indexOf(" contains ") !== -1) {
            var index = 0;
            while(parts.indexOf("contains") != -1) {
                index = parts.indexOf("contains");
                if (index+1 < parts.length && index-1 >= 0) {
                    parts[index-1] = parts[index-1] + ".indexOf(" + parts[index+1] + ") !== -1";
                    parts.splice(index, 2);
                }
            }
        }
        
        condition = parts.join(" ");
    }*/
}

belle.extend(Branch, Action);

Branch.prototype.execute = function()
{
  this.result = eval(this.condition);
  this.actions = (this.result === true) ? this.trueActions : this.falseActions;
  this.actions = this.actions.slice(0);
  if (this.actions.length)
    this.actions[0].execute();
  else
    this.setFinished(true);
}

Branch.prototype.actionFinished = function(action)
{  
  if (this.actions.length)
    this.actions.shift();
  
  if (this.actions.length)
    this.actions[0].execute();
  else
    this.setFinished(true);
}

Branch.prototype.setFinished = function(finished)
{
  if (! finished) {
    var actions = this.trueActions.concat(this.falseActions);
    for(var i=0; i < actions.length; i++) {
      actions[i].setFinished(finished);
    }
  }
  
  Action.prototype.setFinished.call(this, finished);
}

Branch.prototype.skip = function(finished)
{
  if (this.actions.length) {
    this.actions[0].skip();
    return;
  }
  
  Action.prototype.skip.call(this);
}

Branch.prototype.loadCondition = function(condition)
{
    var symbol = "";
    var name = "";
    var _in = "in";
    var _and = "and";
    var _or = "or";
    var validchar = /^[a-zA-Z_]$/g;
    var number = /^[0-9]|([0-9]*\.[0-9]+)$/g;
    var variable_pattern = /^[a-zA-Z_]+[a-zA-Z_0-9]*$/;
    var c = "";
    var i = 0;
    var conditionParts = [];
    var symbols = ["==", "!=", ">" , ">=", "<", "<=", "&&", "||"];
    var string = false;
    condition = condition.split("");
  
    for(i=0; i < condition.length; i++) {
        c = condition[i];
        
        if (c == '"' || c == '\'')
            string = !string;

        //if string add everything to the same value
        if (string) {
            name += c;
            continue;
        }

        if (c.search(validchar) != -1 || c.search(number) != -1 || c == '"' || c == '\'') {
            if (! name) {
                if (condition.slice(i, _in.length) == _in) {
                    i += _in.length;
                    conditionParts.push(_in);
                }
                else if (condition.slice(i, _and.length) == _and) {
                    i += _and.length;
                    conditionParts.push(_and);
                }
                else if (condition.slice(i, _or.length) == _or) {
                    i += _or.length;
                    conditionParts.push(_or);
                }
                else
                    name += c;
            }
            else
                name += c;
        }
        else if (c == '.') {
          if (! string) {
            if (name){
                if (parseInt(name) === NaN)
                    break;
            }
            else
                name += "0";
          }
          name += c;
        }
        else {

            if (name) {
                if (name.search(variable_pattern) != -1)
                  conditionParts.push("game.getValue('" + name + "')");
                else
                  conditionParts.push(name);
                name = "";
            }
             
            if (c == ' ') {
                //do nothing :)
            }
            else if (c == '&' || c == '|') {
                symbol += c;
                if (i+1 < condition.length && condition[i+1] == c) {
                    symbol += condition[i+1];
                    ++i;
                }
            }
            else if (c == '>' || c == '=' || c == '<' || c == '!') {
                symbol += c;
                //in case of ">=" or "<="
                if (i+1 < condition.length && condition[i+1] == '=') {
                    symbol += condition[i+1];
                    ++i;
                }
            }
            else
                continue;

            if (symbols.indexOf(symbol) != -1) {
                conditionParts.push(symbol);
                symbol = "";
            }
        }
    }
    
    if (name) {
        if (name.search(variable_pattern) != -1)
          conditionParts.push("game.getValue('" + name + "')");
        else
          conditionParts.push(name);
    }
    
    this.condition = conditionParts.join(" ");
}

/************* Change Color *****************/

function ChangeColor(data, parent)
{
    Action.call(this, data, parent);
    this.color = new Color([255, 255, 255, 255]);
    this.previousObjectColor = null;
    this.previousObjectBackgroundColor = null;
    this.changeObjectColor = true;
    this.changeObjectBackgroundColor = false;
    
    if ( "color" in data) {
        this.color = new Color(data["color"]);
    }
    
    if ( "changeObjectColor" in data) {
        this.changeObjectColor = data["changeObjectColor"];
    }
    
    if ( "changeObjectBackgroundColor" in data) {
        this.changeObjectBackgroundColor = data["changeObjectBackgroundColor"];
    }
}

belle.extend(ChangeColor, Action);

ChangeColor.prototype.execute = function()
{    
    var object = this.getObject();
    if (! object) {
        this.setFinished(true);
        return;
    }
    
    if (this.changeObjectColor) {
        object.color = this.color;
    }
    
    if (this.changeObjectBackgroundColor) {
        object.setBackgroundColor(this.color);
    }
    
    this.setFinished(true);
}

ChangeColor.prototype.reset = function()
{
    Action.prototype.reset.call(this);
    var object = this.getObject();
    
    if (this.changeObjectColor && this.previousObjectColor) {
        object.setColor(this.previousObjectColor);
    }
    
    if (this.changeObjectBackgroundColor && this.previousObjectBackgroundColor) {
        object.setBackgroundColor(this.previousObjectBackgroundColor);
    }
    
    if (this.changeObjectColor || this.changeObjectBackgroundColor)
        object.redraw = true;
    
    this.setFinished(false);
}

/************* Play Sound *****************/

function PlaySound(data, parent)
{
    Action.call(this, data, parent);
    
    this.soundPath = "";
    this.soundName = "";
    this.sound = null;
    this.loop = false;
    this.formats = [];
    this.volume = 100;
    var ext = "";
    
    if ( "sound" in data) {
        this.soundPath = belle.game.directory + data["sound"];
        this.soundName = this.soundPath;
        if (this.soundPath.indexOf("/") !== -1) {
            var parts = this.soundPath.split("/");
            this.soundName = parts[parts.length-1];
        }
        if (this.soundName.indexOf(".") !== -1)
            this.soundName = this.soundName.split(".")[0];
    }
    
    if ("loop" in data) {
        this.loop = data["loop"];
    }
    
    if ("volume" in data)
        this.volume = data["volume"]; 
    
    if ("formats" in data) {
        this.formats = data["formats"];
    }  
        
}

belle.extend(PlaySound, Action);

PlaySound.prototype.execute = function()
{   
    if (! this.soundPath) {
        this.setFinished(true);
        return;
    }
    
    var options = {
        "loop" : this.loop,
        "volume" : this.volume
    }
    
    this.getGame().playSound(this.soundPath, options);
    
    this.setFinished(true);
}

/************* Stop Sound *****************/

function StopSound(data, parent)
{
    Action.call(this, data, parent);
    
    this.fade = 0;
    this.sound = null;
    
    //can be the name of the file or the name of the action playing it
    if ( "sound" in data) 
        this.soundPath = data["sound"];
    
    if ("fade" in data && typeof data["fade"] === "number") 
        this.fade = data["fade"] * 1000;
        
}

belle.extend(StopSound, Action);

StopSound.prototype.execute = function()
{
    if (this.soundPath)
        game.stopSound(this.soundPath, this.fade);
    
    this.setFinished(true);
}

/************* Show Menu *****************/

function ShowMenu(data, parent)
{
    Action.call(this, data, parent);
    this.options = 0;
    this.skippable = false;
    var object = this.getObject();
   
    if ( "options" in data && typeof data["options"] == "number") 
        this.options = options; 
    
    if (object && object.objects.length) {
      var buttons = object.objects;
      for(var i=0; i < buttons.length; i++) {
        buttons[i].bind("mouseUp", this, function() {
            var scene = this.getScene();
            scene.removeObject(this.object);
            this.setFinished(true);
        }, true);
      }
    }
}

belle.extend(ShowMenu, Action);

ShowMenu.prototype.execute = function()
{
    this.reset();
    var scene = this.getScene();
    var object = this.getObject();
    if (object && scene) {
      object.setVisible(true);
      scene.addObject(object, true);
    }
}

/************* End Novel *****************/

function End(data, parent)
{
    Action.call(this, data, parent);
}

belle.extend(End, Action);

End.prototype.execute = function()
{
    this.getGame().setFinished(true);
    this.setFinished(true);
}

/************* Get user input *****************/

function GetUserInput(data, parent)
{
    Action.call(this, data, parent);
    this.variable = null;
    this.defaultValue = "";
    this.message = "Insert value here:";
    
    if ( "variable" in data) {
        this.variable = data["variable"];
    }
    
    if ( "message" in data ) {
        this.message = data["message"];
    }
    
    if ( "defaultValue" in data ) {
        this.defaultValue = data["defaultValue"];
    }
    
}

belle.extend(GetUserInput, Action);

GetUserInput.prototype.execute = function()
{
    this.reset();
   
    if (! this.variable) {
        this.setFinished(true);
        return;
    }
    var self = this;
    setTimeout(function() {
      var value = prompt(self.message, self.defaultValue);
      if (! value)
        value = self.defaultValue;
      self.getGame().addVariable(self.variable, value);
    
      self.setFinished(true);
    }, 1);
}

/************* Change Game Variable *****************/

function ChangeGameVariable(data, parent)
{
    Action.call(this, data, parent);
    
    this.variable = "";
    this.operation = "";
    this.value = "";
    this.validOperators = ["assign", "add", "subtract", "multiply", "divide", "append"];
    
    if ("variable" in data)
        this.variable = data["variable"];
    
    if ("operator" in data)
        this.operator = data["operator"];
    
    if ("value" in data)
        this.value = data["value"];
    
    if (! this.validOperators.contains(this.operator))
        error("PropertyError: Invalid operator '" + this.operator  + "'");
    

}

belle.extend(ChangeGameVariable, Action);

ChangeGameVariable.prototype.execute = function()
{   
    var currValue = "";
    var newValue = this.value;
    if (this._game.hasVariable(this.variable))
        currValue = this._game.getVariableValue(this.variable);
    
    //if arithmetic operation
    if (this.validOperators.slice(1,5).contains(this.operator)) {
        if (typeof currValue == "string") {
          if (currValue == "")
            currValue = "0";
          currValue = parseFloat(currValue);
        }
        
        if (typeof newValue == "string")
            newValue = parseFloat(newValue);
    }
 
    switch(this.operator) {
        case "assign": 
            currValue = newValue;
            break;
        case "add": 
            if (currValue != NaN && newValue != NaN)
                currValue += newValue;
            break;    
        case "subtract": 
            if (currValue != NaN && newValue != NaN)
                currValue -= newValue;
            break;
        case "multiply": 
            if (currValue != NaN && newValue != NaN)
                currValue *= newValue;
            break;
        case "divide":
            if (currValue != NaN && newValue != NaN && newValue != 0)
                currValue /= newValue;
            break;
        case "append":
            currValue += newValue + "";
            break;
    }
    
    this._game.addVariable(this.variable, currValue);

    this.setFinished(true);
}

/************* Run Script *****************/

function RunScript(data, parent)
{
    Action.call(this, data, parent);
    
    this.code = "";
    
    if ("script" in data)
        this.code = data["script"];
}

belle.extend(RunScript, Action);

RunScript.prototype.execute = function()
{   
    eval(this.code);
    this.setFinished(true);
}

//Expose objects
actions.Action = Action;
actions.Fade = Fade;
actions.Slide = Slide;
actions.Dialogue = Dialogue;
actions.Wait = Wait;
actions.Show = Show;
actions.Hide = Hide;
actions.ChangeBackground = ChangeBackground;
actions.ChangeState = ChangeState;
actions.Label = Label;
actions.GoToLabel = GoToLabel;
actions.GoToScene = GoToScene;
actions.Branch = Branch;
actions.End = End;
actions.ChangeColor = ChangeColor;
actions.PlaySound = PlaySound;
actions.StopSound = StopSound;
actions.ShowMenu = ShowMenu;
actions.GetUserInput = GetUserInput;
actions.ChangeGameVariable = ChangeGameVariable;
actions.RunScript = RunScript;

}(belle.actions));

belle.log("Actions module loaded!");

