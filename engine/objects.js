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

function Object(info, parent, initElement)
{
    Frame.call(this, info, parent);

    this.element = document.createElement("div");
    this.backgroundElement = document.createElement("div");
    this.element.appendChild(this.backgroundElement);

    this.roundedRect = false;
    this.mousePressActions = [];
    this.mouseReleaseActions = [];
    this.mouseMoveActions = [];
    this.mouseLeaveActions = [];
    this.defaultState = null;
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

    var actions;
    var action;
    var actionObject;

    if ("onMousePress" in info) {
        this.mousePressActions = this.initActions(info["onMousePress"]);
    }

    if ("onMouseRelease" in info) {
        this.mouseReleaseActions = this.initActions(info["onMouseRelease"]);
    }

    if ("onMouseMove" in info) {
        this.mouseMoveActions = this.initActions(info["onMouseMove"]);
    }

    if ("onMouseLeave" in info) {
        this.mouseLeaveActions = this.initActions(info["onMouseLeave"]);
    }

    Object.prototype.load.call(this, info);

    if(initElement || initElement === undefined)
      this.initElement();
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

Object.prototype.setCornerRadius = function(radius) {
    this.cornerRadius = radius;
    belle.utils.setBorderRadius(this.element, radius);
    belle.utils.setBorderRadius(this.backgroundElement, radius);
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
  $(this.element).css("border-color", borderColor.toHex());
}

Object.prototype.setBorderWidth = function(borderWidth)
{
  this.borderWidth = borderWidth;
  $(this.element).css("border-width", borderWidth+"px");
  $(this.element).css("border-style", "solid");
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

    if (this.element) {
        $(this.element).css("opacity", this.getOpacityF());
    }
}

Object.prototype.paint = function(context)
{
    if (! this.visible)
      return;

    var x = this.globalX(),
        y = this.globalY(),
        border = this.hasBorder();

    context.globalAlpha = this.getOpacityF();

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
    if (! this.visible)
      return;

    //make sure mousemove actions are stopped
    var actions = this.mouseMoveActions;
    for(var i =0; i !== actions.length; i++)
        actions[i].skip();

    if (this.defaultState)
      this.load(this.defaultState);

    this.hovering = false;
}

Object.prototype.mouseEnterEvent = function(ev)
{
    if (! this.visible)
      return;

    if (this.mouseMoveActions && this.mouseMoveActions.length ||
      this.eventListeners["mousemove"] && this.eventListeners["mousemove"].length)
      this.defaultState = this.serialize();

    this.processEvent(ev, "mouseMove");
    this.hovering = true;
}

Object.prototype.mouseDown = function(event)
{
  return this.processEvent(event, "mouseDown");
}

Object.prototype.mouseUp = function(event)
{
  return this.processEvent(event, "mouseUp");
}

Object.prototype.mouseMove = function(event)
{
    if (this.hovering)
      return false;

    return this.processEvent(event, "mouseMove");
}

Object.prototype.processEvent = function(event, type)
{
    var x = event.canvasX;
    var y = event.canvasY;

    if (! this.contains(x, y))
        return false;

    var actions = [],
        triggered = false;

    if (type == "mouseMove") {
        actions = this.mouseMoveActions;
    }
    else if (type == "mouseUp") {
        actions = this.mouseReleaseActions;
    }
    else if (type == "mouseDown") {
        actions = this.mousePressActions;
    }

    for(var i =0; i !== actions.length; i++)
        actions[i].execute();

    triggered = this.trigger(type);

    if (actions.length || triggered)
      return true;
    return false;
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
        actionInstances.push(action);
    }

    return actionInstances;
}

Object.prototype.initElement = function()
{
  if (! this.element)
    return;

  $(this.element).css("position", "absolute");
  $(this.element).width(this.width);
  $(this.element).height(this.height);
  $(this.backgroundElement).addClass("background");

  var $children = $(this.element).children();
  $children.css("position", "absolute");
  $children.css("width", $(this.element).width()+"px");
  $children.css("height", $(this.element).height()+"px");
  $children.css("display", "block");
  $children.css("filter", "inherit");
  $(this.backgroundElement).css({"width": "100%", "height": "100%"});
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
function Image (data, parent, initElement)
{
    Object.call(this, data, parent, false);
    this.interval = null;
    this.currentFrame = 0;
    this.image = null;

    if(initElement || initElement === undefined)
      this.initElement();

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
  if (! img)
    return;

  var oldImg = this.image,
      assetManager = this.getGame().getAssetManager();

  if (! assetManager)
    return;

  if (typeof img == "string") {
    if (! this.image || this.image.img.src != img)
      this.image = assetManager.loadAsset(img, "Image");
    this._bindImage(this.image);
  }
  else if (belle.isInstance(img, GraphicImage)) {
    this.image = img;
  }

  if (this.image != oldImg) {
    this._unbindImage(oldImg);
    this.update();
    if (this.element) {
      $(this.element).find("img").remove();
      $(this.element).append(this.image.img);
    }
  }
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

function Character(data, parent, initElement)
{
    Image.call(this, data, parent, false);

    var path = "";
    var image;
    var state = null;
    var currState = data["state"];
    var assetManager = this.getGame().getAssetManager();
    this.states = {};
    this.nameColor = null;
    this.textColor = null;

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

    if(initElement || initElement === undefined)
      this.initElement();
}

belle.extend(Character, Image);

Character.prototype.setState = function(state)
{
    if (state in this.states) {
      this.setImage(this.states[state]);
    }
}

/*********** TEXT BOX ***********/

function TextBox(info, parent, initElement)
{
    Object.call(this, info, parent, false);
    this.textLeftPadding = [];
    this.textTopPadding = [];
    this.textAlignment = [];
    this.textParts = [];
    this.textElement = document.createElement("div");
    this.element.appendChild(this.textElement);
    this.displayedText = "";
    var game = this.getGame();

    game.bind("variableChanged", this, function() {
      this.update();
    });

    if ("font" in info)
      this.setFont(info["font"]);
    else
      this.setFont(game.properties.font);

    TextBox.prototype.load.call(this, info);

    if(initElement || initElement === undefined)
      this.initElement();
}

belle.extend(TextBox, Object);

TextBox.prototype.load = function(data)
{
    var loaded = Object.prototype.load.call(this, data);

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

    var text = game ? game.replaceVariables(this.text) : this.text;
    if (text != this.displayedText)
        this.alignText();
    this.displayedText = text;

    for (var i=this.textParts.length-1; i !== -1; --i) {
        context.fillText(this.textParts[i], x+this.textLeftPadding[i], y+this.textTopPadding[i], this.width);
    }

    context.font = defaultFont;
}

TextBox.prototype.alignText = function(text, size)
{
    if (! text && ! this.text)
        return;
    var game = this.getGame();
    if (! text)
        text = game ? game.replaceVariables(this.text) : this.text;

    if (belle.display.DOM && this.textElement) {
        text = text.replace("\n", "<br/>");
        if (this.textAlignment.contains("HCenter"))
            this.textElement.style.textAlign = "center";
        else if (this.textAlignment.contains("Right"))
            this.textElement.style.textAlign = "right";
        else
            this.textElement.style.textAlign = "left";

        if (this.textAlignment.contains("VCenter")) {
            this.textElement.style.top = "0";
            this.textElement.style.verticalAlign = "middle";
        }
        else if (this.textAlignment.contains("Bottom")) {
          this.textElement.style.top = "auto";
          this.textElement.style.bottom = "0";
        }
    }
    else {
        this.textParts = belle.utils.splitText(text, this.font, this.width);
        this.textLeftPadding.length = 0;
        this.textTopPadding.length = 0;
        var sumHeight = 0;

        for (var i=0; i < this.textParts.length; i++) {
            //empty text means it's just a new line, so we use <br/> to get the height
            var text = this.textParts[i].length ? this.textParts[i] : "<br/>";
            var size = belle.utils.textSize(text, this.font);
            var width = size[0];
            var height = Math.round(size[1] / 1.2);
            var leftPadding = 0;
            sumHeight += height;
            this.textTopPadding.push(sumHeight);

            if (this.textAlignment) {
                if (width < this.width) {
                    if (this.textAlignment.contains("HCenter")) {
                        leftPadding = Math.round((this.width - width) / 2);
                    }
                    else if (this.textAlignment.contains("Right")) {
                        leftPadding = this.width - width;
                    }
                }
            }

            this.textLeftPadding.push(leftPadding);
        }

        if (sumHeight < this.height) {
            var topOffset = 0;
            if (this.textAlignment.contains("VCenter"))
                topOffset = (this.height - sumHeight) / 2;
            else if (this.textAlignment.contains("Bottom"))
                topOffset = this.height - sumHeight;
            topOffset = topOffset > 0 ? topOffset : 0;

            topOffset = Math.round(topOffset);
            for(var i=0; i < this.textTopPadding.length; i++)
                this.textTopPadding[i] += topOffset;
        }
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
    if (font && this.font != font) {
        this.font = font;
        if (this.textElement)
          this.textElement.style.font = font;
        this.alignText();
        this.update();
    }
}

TextBox.prototype.setText = function(text)
{
    if (this.text != text) {
        this.text = text;
        var game = this.getGame();
        this.textElement.innerHTML = game ? game.replaceVariables(text.replace("\n", "<br/>")) : text;
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

    if (this.textElement) {
        this.textElement.style.color = color.toHex();
    }

    this.update();
}

TextBox.prototype.activateDefaultTextColor = function()
{
    this.textColor = this.defaultTextColor;
}

TextBox.prototype.scale = function(widthFactor, heightFactor)
{
  Object.prototype.scale.call(this, widthFactor, heightFactor);

  if (this.textElement) {
    this.textElement.style.width = this.width + "px";
    this.textElement.style.height = this.height + "px";
    var font = parseInt(this.font);
    if (font !== NaN) {
        font *= widthFactor;
        $(this.textElement).css("font-size", font+"px");
    }
  }
  this.alignText();
}

TextBox.prototype.serialize = function()
{
    var data = Object.prototype.serialize.call(this);
    data["text"] = this.text;
    if (this.textColor)
      data["textColor"] = this.textColor.serialize();
    data["textAlignment"] = this.textAlignment.join("|");

    return data;
}

TextBox.prototype.update = function()
{
    Object.prototype.update.call(this);
    var game = this.getGame();
    if (this.textElement && game)
      this.textElement.innerHTML = game.replaceVariables(this.text);
}

TextBox.prototype.initElement = function()
{
  Object.prototype.initElement.call(this);

  this.textElement.style.display = "table-cell";
  this.textElement.style.position = "relative";
}

/*********** Object Group ***********/

function ObjectGroup(data, parent, initElement)
{
    Object.call(this, data, parent, false);
    this.objects = [];
    this.hoveredObject = null;

    ObjectGroup.prototype.load.call(this, data);

    if(initElement || initElement === undefined)
      this.initElement();

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
            var left = parseInt(this.element.style.left);
            var elemLeft = parseInt(obj.element.style.left);
            //TODO: position should be relative to parent or global?
            //obj.setX(obj.x - this.x);



            var top = parseInt(this.element.style.top);
            var elemTop = parseInt(obj.element.style.top);
            //obj.setY(obj.y - this.y);

            //if (belle.display.DOM) {
                obj.element.style.display = "block";
                obj.element.style.left = obj.x + "px";
                //temporary hack in DOM mode to fix top/y value
                obj.element.style.top = obj.y + "px";
                this.element.appendChild(obj.element);
            //}

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

ObjectGroup.prototype.objectAt = function(x, y)
{
    for(var i=0; i !== this.objects.length; i++) {
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
  Object.prototype.mouseLeaveEvent.call(this, event);

  if (this.hoveredObject) {
    this.hoveredObject.mouseLeaveEvent(event);
    this.hoveredObject = null;
  }
}

ObjectGroup.prototype.processEvent = function(event, type)
{
    var x = event.canvasX;
    var y = event.canvasY;

    if (! this.visible || ! this.contains(x, y))
        return false;

    var result = Object.prototype.processEvent.call(this, event);
    var object = this.objectAt(x, y);


    if (this.hoveredObject != object) {
      if (this.hoveredObject)
        this.hoveredObject.mouseLeaveEvent(event);

      if (object)
        object.mouseEnterEvent(event);
    }

    this.hoveredObject = object;

    if (object) {
      result = object.processEvent(event, type);
    }

    return result;
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

function DialogueBox(data, parent, initElement)
{
    ObjectGroup.call(this, data, parent, initElement);

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

/************** MENU ************/

function Menu(data, parent, initElement)
{
    ObjectGroup.call(this, data, parent, initElement);
}

belle.extend(Menu, ObjectGroup);


/************** BUTTON ************/

function Button(data, parent, initElement)
{
    TextBox.call(this, data, parent, initElement);
    this.visible = true;
    if (this.element)
      this.element.style.cursor = "pointer";

}

belle.extend(Button, TextBox);

// Expose the public methods

objects.Object = Object;
objects.Image = Image;
objects.TextBox = TextBox;
objects.Character = Character;
objects.ObjectGroup = ObjectGroup;
objects.DialogueBox = DialogueBox;
objects.Button = Button;
objects.Menu = Menu;

}(belle));

belle.log("Objects module loaded!");
