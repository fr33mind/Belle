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

var Color = belle.graphics.Color,
  Frame = belle.Frame,
  GameObject = belle.GameObject,
  GraphicImage = belle.graphics.Image,
  AnimatedImage = belle.graphics.AnimatedImage,
  objects = belle.objects;

/*********** OBJECT ***********/

function Object(info, parent)
{
    Frame.call(this, info, parent);

    this.roundedRect = false;
    this.mousePressActionGroup = null;
    this.mouseReleaseActionGroup = null;
    this.mouseMoveActionGroup = null;
    this.mouseLeaveActionGroup = null;
    this.redrawing = false;
    this.hovering = false;
    this.loaded = true;
    this.data = info;
    this.resource = info["resource"] || "";
    this.eventListeners = {
        "mousepress" : [],
        "mouserelease" : [],
        "mousemove" : []
    };

    var actions,
        action,
        actionGroup;

    if ("onMousePress" in info) {
        this.mousePressActionGroup = this.initActionGroup(info["onMousePress"]);
    }

    if ("onMouseRelease" in info) {
        this.mouseReleaseActionGroup = this.initActionGroup(info["onMouseRelease"]);
    }

    if ("onMouseMove" in info) {
        this.mouseMoveActionGroup = this.initActionGroup(info["onMouseMove"]);
    }

    if ("onMouseLeave" in info) {
        this.mouseLeaveActionGroup = this.initActionGroup(info["onMouseLeave"]);
    }

    Object.prototype.load.call(this, info);
}

belle.extend(Object, Frame);

Object.prototype.load = function(data)
{
    if (! data)
      return false;

    Frame.prototype.load.call(this, data);

    this.cornerRadius = 0;
    this.opacity = 255;
    this.borderWidth = 0;
    this.borderColor = null;

    if ("cornerRadius" in data && data["cornerRadius"] > 0) {
      this.setCornerRadius(data["cornerRadius"]);
    }

    if ("borderWidth" in data) {
      this.setBorderWidth(data["borderWidth"]);
    }

    if ("borderColor" in data) {
      this.setBorderColor(new Color(data["borderColor"]));
    }

    if ("opacity" in data) {
        this.setOpacity(data["opacity"]);
    }

    return true;
}

Object.prototype.addEventAction = function(event, action)
{
  if (event == "mouseMove") {
    if (! this.mouseMoveActionGroup)
      this.mouseMoveActionGroup = new belle.actions.ActionGroup({}, this);
    this.mouseMoveActionGroup.addAction(action);
  }
  else if (event == "mouseUp") {
    if (! this.mouseReleaseActionGroup)
      this.mouseReleaseActionGroup = new belle.actions.ActionGroup({}, this);
    this.mouseReleaseActionGroup.addAction(action);
  }
  else if (event == "mouseDown") {
    if (! this.mousePressActionGroup)
      this.mousePressActionGroup = new belle.actions.ActionGroup({}, this);
    this.mousePressActionGroup.addAction(action);
  }
}

Object.prototype.setCornerRadius = function(radius) {
    this.cornerRadius = radius;
    if (this.cornerRadius > 0)
      this.roundedRect = true;
}

Object.prototype.globalOuterX = function()
{
  return this.globalX() - this.borderWidth;
}

Object.prototype.globalOuterY = function()
{
  return this.globalY() - this.borderWidth;
}

Object.prototype.outerX = function()
{
  return this.x - this.borderWidth;
}

Object.prototype.outerY = function()
{
  return this.y - this.borderWidth;
}

Object.prototype.outerWidth = function()
{
  return this.width + this.borderWidth * 2;
}

Object.prototype.outerHeight = function()
{
  return this.height + this.borderWidth * 2;
}

Object.prototype.overlaps = function(object)
{
    if ( object.outerX() > this.outerX() + this.outerWidth() || object.outerY() > this.outerY() + this.outerHeight() ||
      this.outerX() > object.outerX() + object.outerWidth() || this.outerY() > object.outerY() + object.outerHeight() )
        return false;

    return true;
}

Object.prototype.overlaped = function(object)
{
    if (object.paintX === false || this.paintX === false)
      return false;

    //paint* variables already contain the proper position and size values
    if ( object.paintX > this.paintX + this.paintWidth || object.paintY > this.paintY + this.paintHeight ||
      this.paintX > object.paintX + object.paintWidth || this.paintY > object.paintY + object.paintHeight )
        return false;

    return true;
}

Object.prototype.setBorderColor = function(borderColor)
{
  this.borderColor = borderColor;
}

Object.prototype.setBorderWidth = function(borderWidth)
{
  this.borderWidth = borderWidth;
}

Object.prototype.getOpacity = function (alpha)
{
  return this.opacity;
}

Object.prototype.getOpacityF = function (alpha)
{
  return this.opacity / 255;
}

Object.prototype.setOpacity = function (alpha)
{
    if (alpha < 0)
        alpha = 0;
    else if (alpha > 255)
        alpha = 255;

    if (this.opacity != alpha) {
      this.opacity = alpha;
      this.redraw = true;
    }
}

Object.prototype.paint = function(context)
{
    if (! this.visible)
      return;

    var x = this.globalX(),
        y = this.globalY(),
        border = this.hasBorder();

    context.globalAlpha = this.getOpacityF() * context.globalAlpha;

    if (border) {
        context.lineWidth = this.borderWidth;
        context.strokeStyle = this.borderColor.toString();
    }

    if (this.roundedRect) {
        var width = this.width;
        var height = this.height;
        var xradius = this.cornerRadius;
        var yradius = this.cornerRadius;

        context.beginPath();

        context.moveTo(x+xradius, y);
        context.lineTo(x+width-xradius, y); //to work with IE
         //draw top and top right corner
        context.arcTo(x + width, y, x + width, y + yradius, yradius);

        //to work with IE
        context.lineTo(x+width, y+height-yradius);
        //draw right and bottom right corner
        context.arcTo(x + width, y+height, x+width-xradius, y + height, xradius);

        //to work with IE
        context.lineTo(x+xradius, y+height);
        //draw bottom and bottom left corner
        context.arcTo(x, y+height, x, y+height-yradius, yradius);

        //to work with IE
        context.lineTo(x, y+yradius);
        //draw left and top left corner
        context.arcTo(x, y, x+xradius, y, xradius);

        context.closePath();
        if (border)
          context.stroke();
        context.clip();
    }
    else {
        context.beginPath();
        context.rect(x, y, this.width, this.height);
        context.closePath();
        if (border)
          context.stroke();
    }

    //draw background
    Frame.prototype.paint.call(this, context);

    this.paintX = this.globalOuterX();
    this.paintY = this.globalOuterY();
    this.paintWidth = this.outerWidth();
    this.paintHeight = this.outerHeight();
}

Object.prototype.hasBorder = function()
{
  return (this.borderWidth > 0 && this.borderColor);
}

Object.prototype.mouseLeaveEvent = function(ev)
{
    if (! this.visible || ! this.hovering)
      return;

    //make sure mousemove actions are stopped
    if (this.mouseMoveActionGroup)
      this._gameModel.stopAction(this.mouseMoveActionGroup);

    this.hovering = false;
}

Object.prototype.mouseEnterEvent = function(ev)
{
    if (! this.visible || this.hovering)
      return;

    this.hovering = true;
    this.processEvent(ev, "mouseMove");
}

Object.prototype.mouseDown = function(event)
{
  this.processEvent(event, "mouseDown");
}

Object.prototype.mouseUp = function(event)
{
  this.processEvent(event, "mouseUp");
}

Object.prototype.mouseMove = function(event)
{
    if (this.hovering)
      return;

    this.processEvent(event, "mouseMove");
}

Object.prototype.processEvent = function(event, type)
{
    var x = event.canvasX,
        y = event.canvasY,
        gameModel = this.getGameModel();

    if (! gameModel || ! this.contains(x, y))
      return;

    var actionGroup = null,
        triggered = false;

    if (type == "mouseMove") {
        actionGroup = this.mouseMoveActionGroup;
    }
    else if (type == "mouseUp") {
        actionGroup = this.mouseReleaseActionGroup;
    }
    else if (type == "mouseDown") {
        actionGroup = this.mousePressActionGroup;
    }

    if (actionGroup)
      gameModel.executeAction(actionGroup);

    triggered = this.trigger(type);

    if (actionGroup || triggered)
      event.processed = true;
}

Object.prototype.needsRedraw = function()
{
    return this.redraw;
}

Object.prototype.scale = function(widthFactor, heightFactor)
{
  this.setX(this.x);
  this.setY(this.y);
  this.setWidth(this.width);
  this.setHeight(this.height);
  this.redraw = true;
}

Object.prototype.initActions = function(actions)
{
    var actionInstances = [];
    var actionInstance = [];
    var action = null;

    for(var i=0; i !== actions.length; i++) {
        action = belle.createAction(actions[i], this);
        if (action) {
          if (! action.object && ! action.objectName)
            action.object = this;
          actionInstances.push(action);
        }
    }

    return actionInstances;
}

Object.prototype.initActionGroup = function(actions)
{
    if (! actions || ! actions.length)
      return null;

    var actions = this.initActions(actions),
        actionGroup = new belle.actions.ActionGroup({}, this);

    actionGroup.addActions(actions);
    return actionGroup;
}

Object.prototype.serialize = function()
{
  var data = Frame.prototype.serialize.call(this);
  data["cornerRadius"] = this.cornerRadius;
  if (this.borderWidth)
    data["borderWidth"] = this.borderWidth;
  if (this.borderColor)
    data["borderColor"] = belle.serialize(this.borderColor);
  data["opacity"] = this.opacity;
  data["visible"] = this.visible;

  return data;
}

/*********** IMAGE OBJECT ***********/
function Image (data, parent)
{
    Object.call(this, data, parent);
    this.interval = null;
    this.currentFrame = 0;
    this.image = null;

    this.load(data);
}

belle.extend(Image, Object);

Image.prototype.load = function(data)
{
  Object.prototype.load.call(this, data);

  this.image = null;

  if ("image" in data)
    this.setImage(data["image"]);
}

Image.prototype.setImage = function(img)
{
  if (this.image == img)
    return;

  var oldImg = this.image,
      assetManager = this.getGame().getAssetManager();

  if (! assetManager)
    return;

  if (typeof img == "string") {
    this.image = assetManager.loadAsset(img, "Image");
  }
  else if (belle.isInstance(img, GraphicImage)) {
    this.image = img;
  }
  else {
    this.image = null;
  }

  if (this.image != oldImg) {
    this._bindImage(this.image);
    this._unbindImage(oldImg);
    this.update();
  }

  assetManager.releaseAsset(oldImg);
}

Image.prototype.paint = function(context)
{
    if (! this.visible)
      return;

    Object.prototype.paint.call(this, context);

    if (this.image) {
      context.drawImage(this.image.getElement(), this.globalX(), this.globalY(), this.width, this.height);
    }
}

Image.prototype.isReady = function()
{
    var ready = Object.prototype.isReady.call(this);
    if (! ready)
        return ready;

    if (this.image)
        return this.image.isReady();

    return true;
}

Image.prototype.setVisible = function(visible)
{
  Frame.prototype.setVisible.call(this, visible);
  if (this.image && this.image.isAnimated()) {
    if (this.visible)
      this.image.play();
    else
      this.image.stop();
  }
}

Image.prototype.serialize = function()
{
  var data = Object.prototype.serialize.call(this);
  if (this.image)
    data["image"] = this.image.path;

  return data;
}

/*********** CHARACTER ***********/

function Character(data, parent)
{
    Image.call(this, data, parent);

    var path = "";
    var image;
    var state = null;
    var currState = data["state"];
    var assetManager = this.getGame().getAssetManager();
    this.states = {};
    this.nameColor = null;
    this.textColor = null;
    this.currentState = currState;

    if ("states" in data) {
        for(state in data["states"]) {
            if (state != currState) {
              image = assetManager.loadAsset(data.states[state], "Image");
              this.states[state] = image;
            }
        }

        this.setImage(data.states[currState]);
        this.states[currState] = this.image;
    }

    if ("nameColor" in data)
      this.nameColor = new Color(data["nameColor"]);
    if ("textColor" in data)
      this.textColor = new Color(data["textColor"]);
}

belle.extend(Character, Image);

Character.prototype.setState = function(state)
{
    if (state in this.states) {
      this.currentState = state;
      this.setImage(this.states[state]);
    }
}

/*********** TEXT BOX ***********/

function TextBox(info, parent)
{
    Object.call(this, info, parent);
    this.textLeftPadding = [];
    this.textTopPadding = [];
    this.textAlignment = [];
    this.textParts = [];
    this.font = null;
    this.displayedText = "";
    var game = this.getGame();

    game.bind("variableChanged", this, function() {
      this.update();
    });

    TextBox.prototype.load.call(this, info);
}

belle.extend(TextBox, Object);

TextBox.prototype.load = function(data)
{
    var loaded = Object.prototype.load.call(this, data);
    var game = this.getGame();

    if (! loaded)
      return false;

    if ("textAlignment" in data)
      this.textAlignment = data["textAlignment"].split("|");

    var text = "";
    if ("text" in data)
      text = data["text"];
    this.setText(text);

    if ("textColor" in data) {
        this.setTextColor(data["textColor"]);
        this.defaultTextColor = this.textColor;
    }

    if ("font" in data) {
      this.setFont(data["font"]);
    }
    else if (game.properties.font) {
      this.setFont(game.properties.font);
    }

    return true;
}

TextBox.prototype.paint = function(context)
{
    if (! this.visible)
      return;

    Object.prototype.paint.call(this, context);

    var x = this.globalX();
    var y = this.globalY();
    context.fillStyle = this.textColor.toString();
    var defaultFont = context.font;
    var game = this.getGame();

    if (this.font)
       context.font = this.font;

    context.textBaseline = "bottom";

    var text = game ? game.replaceVariables(this.text) : this.text;
    if (text != this.displayedText)
        this.alignText();
    this.displayedText = text;

    for (var i=this.textParts.length-1; i !== -1; --i) {
        context.fillText(this.textParts[i], x+this.textLeftPadding[i], y+this.textTopPadding[i], this.contentWidth());
    }

    context.font = defaultFont;
}

TextBox.prototype.alignText = function()
{
    var game = this.getGame();
    var text = game ? game.replaceVariables(this.text) : this.text;
    var contentWidth = this.contentWidth();
    var contentHeight = this.contentHeight();
    this.textParts = belle.utils.splitText(text, this.font, contentWidth);
    this.textLeftPadding.length = 0;
    this.textTopPadding.length = 0;
    var sumHeight = 0;

    for (var i=0; i < this.textParts.length; i++) {
        //empty text means it's just a new line, so we use <br/> to get the height
        var text = this.textParts[i].length ? this.textParts[i] : "<br/>";
        var size = belle.utils.textSize(text, this.font);
        var width = size[0];
        var height = size[1];
        var leftPadding = this.padding.left;
        this.textTopPadding.push(sumHeight+height);
        sumHeight += height;

        if (this.textAlignment) {
            if (width < contentWidth) {
                if (this.textAlignment.contains("HCenter")) {
                    leftPadding += Math.round((contentWidth - width) / 2);
                }
                else if (this.textAlignment.contains("Right")) {
                    leftPadding += contentWidth - width;
                }
            }
        }

        this.textLeftPadding.push(leftPadding);
    }

    if (sumHeight < contentHeight) {
        var topOffset = this.padding.top;
        if (this.textAlignment.contains("VCenter"))
            topOffset += (contentHeight - sumHeight) / 2;
        else if (this.textAlignment.contains("Bottom"))
            topOffset += contentHeight - sumHeight;
        topOffset = topOffset > 0 ? topOffset : 0;

        topOffset = Math.round(topOffset);
        for(var i=0; i < this.textTopPadding.length; i++)
            this.textTopPadding[i] += topOffset;
    }
}

TextBox.prototype.needsRedraw = function()
{
    if (this.redraw)
        return true;

    if (! this.visible)
        return false;

    var game = this.getGame();
    var displayText = game ? game.replaceVariables(this.text) : this.text;

    if (displayText != this.displayedText) {
        this.redraw = true;
        return true;
    }

    return false;
}

TextBox.prototype.appendText = function(text)
{
    this.setText(this.text + text);
}

TextBox.prototype.setFont = function(font)
{
  if (!font)
    return;

  var fontLibrary = this.getGame().getFontLibrary();

  if (belle.isInstance(font, belle.graphics.Font))
    this.font = font;
  else if (fontLibrary)
    this.font = fontLibrary.createFont(font);
  else
    this.font = new belle.graphics.Font(font);

  this.alignText();
  this.update();
}

TextBox.prototype.setText = function(text)
{
    if (this.text != text) {
        this.text = text;
        this.alignText();
        this.update();
    }
}

TextBox.prototype.setTextColor = function(color)
{
    if (! color)
      return;

    if (color instanceof Array)
        color = new Color(color);
    this.textColor = color;
    this.update();
}

TextBox.prototype.activateDefaultTextColor = function()
{
    this.textColor = this.defaultTextColor;
}

TextBox.prototype.scale = function(widthFactor, heightFactor)
{
  Object.prototype.scale.call(this, widthFactor, heightFactor);

  this.alignText();
}

TextBox.prototype.serialize = function()
{
    var data = Object.prototype.serialize.call(this);
    data["text"] = this.text;
    if (this.textColor)
      data["textColor"] = this.textColor.serialize();
    data["textAlignment"] = this.textAlignment.join("|");
    if (this.font)
      data["font"] = this.font.serialize();

    return data;
}

/*********** Object Group ***********/

function ObjectGroup(data, parent)
{
    Object.call(this, data, parent);
    this.objects = [];
    this.hoveredObject = null;

    ObjectGroup.prototype.load.call(this, data);
}

belle.extend(ObjectGroup, Object);

ObjectGroup.prototype.load = function(data) {

    var loaded = Object.prototype.load.call(this, data);

    if (! loaded)
      return false;

    if ("objects" in data) {
        var obj,
            objects = data["objects"],
            game = this.getGame();

        this.objects =  [];

        for (var i=0; i !== objects.length; i++) {
            /*obj = this.getObject(objects[i].name);
            if (obj) {
              obj.load(objects[i]);
              //fix x,y coords
              obj.setX(obj.x - this.x);
              obj.setY(obj.y - this.y);
              continue;
            }*/

            obj = game.createObject(objects[i], this);

            if (! obj) {
                belle.log(objects[i].type + ": is not a valid object type");
                continue;
            }

            this.objects.push(obj);
        }
    }


    return true;
}

ObjectGroup.prototype.serialize = function()
{
    var data = Object.prototype.serialize.call(this);
    data["objects"] = belle.serialize(this.objects);
    return data;
}

ObjectGroup.prototype.getObjectAt = function(x, y)
{
    for(var i=this.objects.length-1; i != -1; i--) {
        if (this.objects[i].contains(x, y)) {
            return this.objects[i];
        }
    }

    return null;
}

ObjectGroup.prototype.paint = function(context)
{
    if (! this.visible)
      return;

    Object.prototype.paint.call(this, context);

    for(var i=0; i !== this.objects.length; i++) {
        this.objects[i].redraw = true;
        context.save();
        this.objects[i].paint(context);
        this.objects[i].redraw = false;
        context.restore();
    }
}

ObjectGroup.prototype.mouseLeaveEvent = function(event)
{
  if (this.hoveredObject) {
    this.hoveredObject.mouseLeaveEvent(event);
    var game = this.getGame();
    if (game)
      game.removeWatchedObject(this.hoveredObject);
    this.hoveredObject = null;
  }

  Object.prototype.mouseLeaveEvent.call(this, event);
}

ObjectGroup.prototype.mouseMove = function(event)
{
  var object = this.getObjectAt(event.canvasX, event.canvasY);
  var game = this.getGame();

  if (this.hoveredObject != object) {
    if (this.hoveredObject) {
        this.hoveredObject.mouseLeaveEvent(event);
        if (game)
          game.removeWatchedObject(this.hoveredObject);
    }

    if (object) {
      if (game)
        game.addWatchedObject(object);
      object.mouseEnterEvent(event);
    }
  }
  else if (object) {
    object.mouseMove(event);
  }

  this.hoveredObject = object;
  Object.prototype.mouseMove.call(this, event);
}

ObjectGroup.prototype.mouseEnterEvent = function(event)
{
  this.hoveredObject = this.getObjectAt(event.canvasX, event.canvasY);
  if (this.hoveredObject) {
    var game = this.getGame();
    if (game)
      game.addWatchedObject(this.hoveredObject);
    this.hoveredObject.mouseEnterEvent(event);
  }

  Object.prototype.mouseEnterEvent.call(this, event);
}

ObjectGroup.prototype.mouseDown = function(event)
{
  if (! this.hoveredObject)
    this.hoveredObject = this.getObjectAt(event.canvasX, event.canvasY);
  if (this.hoveredObject)
    this.hoveredObject.mouseDown(event);
  Object.prototype.mouseDown.call(this, event);
}

ObjectGroup.prototype.mouseUp = function(event)
{
  if (! this.hoveredObject)
    this.hoveredObject = this.getObjectAt(event.canvasX, event.canvasY);
  if (this.hoveredObject)
    this.hoveredObject.mouseUp(event);
  Object.prototype.mouseUp.call(this, event);
}

ObjectGroup.prototype.clear = function (context)
{
  Object.prototype.clear.call(this, context);

  for(var i=0; i !== this.objects.length; i++) {
      this.objects[i].clear(context);
  }
}

ObjectGroup.prototype.needsRedraw = function()
{
    for (var i=0; i !== this.objects.length; i++) {
        if (this.objects[i].needsRedraw()) {
            this.redraw = true;
            break;
        }
    }

    return this.redraw;
}

ObjectGroup.prototype.scale = function(widthFactor, heightFactor)
{
  Object.prototype.scale.call(this, widthFactor, heightFactor);

  for (var i=0; i !== this.objects.length; i++) {
    this.objects[i].scale(widthFactor, heightFactor);
  }
}

ObjectGroup.prototype.isReady = function()
{
  var ready = Object.prototype.isReady.call(this);

  if (! ready)
      return false;

  for(var i=0; i < this.objects.length; i++){
    if (! this.objects[i].isReady()) {
        ready = false;
        break;
    }
  }

  return ready;
}

ObjectGroup.prototype.getObject = function(name)
{
  if (name) {
    for (var i=0; i !== this.objects.length; i++) {
      if (this.objects[i].name == name)
        return this.objects[i];
    }
  }

  return null;
}

/*********** DIALOGUE BOX ***********/

function DialogueBox(data, parent)
{
    ObjectGroup.call(this, data, parent);

    this.text = "";
    this.speakerName = "";
    this.speakerTextBox = null;
    this.dialgueTextBox = null;

    for(var i=0; i !== this.objects.length; i++) {
        if (this.objects[i].name === "speakerTextBox")
            this.speakerTextBox = this.objects[i];
        else if (this.objects[i].name === "dialogueTextBox")
            this.dialogueTextBox = this.objects[i];
    }
}

belle.extend(DialogueBox, ObjectGroup);

DialogueBox.prototype.setSpeakerName = function(text)
{
    if (this.speakerTextBox) {
        this.speakerTextBox.setText(text);
        this.redraw = true;
    }
}

DialogueBox.prototype.appendText = function(text)
{
    if (this.dialogueTextBox) {
        this.dialogueTextBox.appendText(text);
        this.redraw = true;
    }
}

DialogueBox.prototype.setText = function(text)
{
    if (this.dialogueTextBox) {
        this.dialogueTextBox.setText(text);
        this.redraw = true;
    }
}

DialogueBox.prototype.setNameColor = function(color)
{
    if (this.speakerTextBox)
      this.speakerTextBox.setTextColor(color);
}

DialogueBox.prototype.setTextColor = function(color)
{
    if (this.dialogueTextBox)
      this.dialogueTextBox.setTextColor(color);
}

DialogueBox.prototype.activateDefaultTextColor = function()
{
    if (this.speakerTextBox)
      this.speakerTextBox.activateDefaultTextColor();
    if (this.dialogueTextBox)
      this.dialogueTextBox.activateDefaultTextColor();
}

/************** BUTTON ************/

function Button(data, parent)
{
    TextBox.call(this, data, parent);
}

belle.extend(Button, TextBox);

/************** MENU OPTION ************/

function MenuOption(data, parent)
{
  Button.call(this, data, parent);
  this.actionGroup = null;
  this.condition = "";

  if ("actions" in data) {
    var actions = data["actions"];
    this.actionGroup = new belle.actions.ActionGroup({}, this);
    for(var i=0; i < actions.length; i++) {
      this.actionGroup.addAction(belle.createAction(actions[i], this));
    }

    this.bind("mouseUp", function() {
      this.actionGroup.execute();
    });
  }

  if ("condition" in data) {
    this.condition = data["condition"];
  }
}

belle.extend(MenuOption, Button);

/************** MENU ************/

function Menu(data, parent)
{
    ObjectGroup.call(this, data, parent);

    if (this.objects) {
      var self = this;
      for(var i=0; i < this.objects.length; i++) {
        this.objects[i].bind("mouseUp", function() {
          self.trigger("optionSelected", this);
          this.getScene().removeObject(self);
        });
      }
    }
}

belle.extend(Menu, ObjectGroup);

Menu.prototype.getOptionAt = function(index)
{
  if (index >= 0 && index < this.objects.length)
    return this.objects[index];
  return null;
}

/************** Sound ************/

function Sound(data, parent)
{
  GameObject.call(this, data, parent);
  this.asset = data.asset || null;

  if (data.asset) {
    var assetManager = this.getGame().getAssetManager();
    if (assetManager)
      this.asset = assetManager.loadAsset(data.asset, "Audio");
  }
}

belle.extend(Sound, GameObject);

/************** Slot Button ************/

function SlotButton(data, parent)
{
    ObjectGroup.call(this, data, parent);
    this.SlotTypes = {
      Save: 0,
      Load: 1
    };

    this.slot = 0;
    this.thumbnailEnabled = true;
    this.emptyThumbnail = null;
    this.slotType = this.SlotTypes.Save;
    this.empty = true;

    this.bind("mouseUp", function() {
      if (this.slotType == this.SlotTypes.Save)
        this.saveGame();
      else if (this.slotType == this.SlotTypes.Load)
        this.loadGame();
    });

    SlotButton.prototype.load.call(this, data);
}

belle.extend(SlotButton, ObjectGroup);

SlotButton.prototype.load = function(data) {
    var loaded = ObjectGroup.prototype.load.call(this, data);

    if (! loaded)
      return false;

    if ("slot" in data)
      this.setSlot(data["slot"]);

    if ("thumbnailEnabled" in data)
      this.thumbnailEnabled = data["thumbnailEnabled"];

    if ("emptyThumbnail" in data) {
      var assetManager = this.getGame().getAssetManager();
      if (assetManager)
        this.emptyThumbnail = assetManager.loadAsset(data["emptyThumbnail"], "Image");
    }

    if ("slotType" in data && belle.isNumber(data["slotType"])) {
      this.slotType = parseInt(data["slotType"]);
    }
}

SlotButton.prototype.setSlot = function(slot) {
  if (!belle.isNumber(slot))
    return;

  this.slot = parseInt(slot);
  var textbox = this.getObject("textbox");
  if (textbox)
    textbox.setText(textbox.text.replace("$id", this.slot));
  this.loadFromSlot();
}

SlotButton.prototype.loadFromSlot = function() {
  this.empty = true;
  var game = this.getGame();
  if (game) {
    var data = game.getSlot(this.slot);
    if (data) {
      var thumbnail = this.getObject("thumbnail");
      if (thumbnail) {
        var img = null;
        if (data.thumbnail) {
          img = new belle.graphics.Image(data.thumbnail, function(){
                                        thumbnail.update();
                                      });
        }
        thumbnail.setImage(img);
      }
      var textbox = this.getObject("textbox");
      if (textbox)
        textbox.setText(this.slot + "." + data.name + "\n" + data.date);
      this.empty = false;
    }
  }
}

SlotButton.prototype.saveGame = function() {
  var game = this.getGame();
  if (game) {
    game.saveSlot(this.slot, null, this.thumbnailEnabled);
    this.loadFromSlot();
  }
}

SlotButton.prototype.loadGame = function() {
  var game = this.getGame();
  if (game) {
    game.loadSlot(this.slot);
  }
}

/************** Font ************/

function Font(data, parent)
{
  GameObject.call(this, data, parent);
  this.asset = data.asset || null;

  if (data.asset) {
    var assetManager = this.getGame().getAssetManager();
    if (assetManager)
      this.asset = assetManager.loadAsset(data.asset, "Font");
  }
}

belle.extend(Font, GameObject);

// Expose the public methods

objects.Object = Object;
objects.Image = Image;
objects.TextBox = TextBox;
objects.Character = Character;
objects.ObjectGroup = ObjectGroup;
objects.DialogueBox = DialogueBox;
objects.Button = Button;
objects.MenuOption = MenuOption;
objects.Menu = Menu;
objects.Sound = Sound;
objects.SlotButton = SlotButton;
objects.Font = Font;

}(belle));

