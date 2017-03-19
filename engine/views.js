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

var Object = belle.core.Object;

function AbstractView(container, parent)
{
    Object.call(this, parent);
    this.model = null;
    this.container = container || $('body');
    this.element = $('<div class="view"></div>');
    this.container.append(this.element);
    this.scaledWidthFactor = 1;
    this.scaledHeightFactor = 1;
    this.width = this.height = 0;
}

belle.extend(AbstractView, Object);

AbstractView.prototype.render = function()
{
}

AbstractView.prototype.clear = function()
{
}

AbstractView.prototype.show = function()
{
  this.element.show();
  this.render();
}

AbstractView.prototype.hide = function()
{
  this.element.hide();
}

AbstractView.prototype.setWidth = function(width)
{
  this.width = width;
  this.element.css("max-width", width+"px");
  this.resize();
}

AbstractView.prototype.setHeight = function(height)
{
  this.height = height;
  this.element.css("max-height", height+"px");
  this.resize();
}

AbstractView.prototype.setSize = function(width, height)
{
  this.width = width;
  this.height = height;
  this.resize();
}

AbstractView.prototype.update = function(data)
{
  this.render();
}

AbstractView.prototype._detectButton = function(e)
{
  var button = "";

  if (e.which == null)
  {
    button = (e.button < 2) ? 'left' :
        ((e.button == 4) ? 'middle' : 'right');
  }
  else
  {
    button = (e.which < 2) ? 'left' :
        ((e.which == 2) ? 'middle' : 'right');
  }

  return button;
}

AbstractView.prototype.mouseUp = function (ev)
{
}

AbstractView.prototype.mouseDown = function (ev)
{
}

AbstractView.prototype.mouseMove = function (ev)
{
}

AbstractView.prototype.keyUp = function (ev)
{
}

AbstractView.prototype.keyDown = function (ev)
{
}

AbstractView.prototype.setModel = function(model)
{
  this.model = model;
}

AbstractView.prototype.getModel = function()
{
  return this.model;
}

AbstractView.prototype.contains = function(x, y) {
    var offset = this.element.offset(),
        width = this.element.width(),
        height = this.element.height();

    if (x < offset.left || y < 0 || x > offset.left+width || y > offset.top+height)
        return false;

    return true;
}

AbstractView.prototype.resize = function(ev) {
  var width = this.width,
      height = this.height,
      top = 0,
      left = 0;

  var screenWidth = this.container.width();
  screenWidth = screenWidth ? screenWidth : width;
  var screenHeight = this.container.height();
  screenHeight = screenHeight ? screenHeight : height;
  var widthFactor = 1;
  var heightFactor = 1;

  //if game screen size is bigger than screen, scale it
  if (width > screenWidth)
      widthFactor = screenWidth / width;

  if (height > screenHeight)
      heightFactor = screenHeight / height;

  if (heightFactor < widthFactor)
      widthFactor = heightFactor;
  else
      heightFactor = widthFactor;

  this.scaledWidthFactor = widthFactor;
  this.scaledHeightFactor = heightFactor;

  width *= widthFactor;
  height *= heightFactor;

  //if game sreen size is smaller than screen, center it, otherwise no padding is added
  if (width < screenWidth )
    left = (screenWidth - width) / 2;

  if (height < screenHeight)
    top = (screenHeight - height) / 2;

  this.element.css("top", top + "px");
  this.element.css("left", left + "px");
  this.element.width(parseInt(width));
  this.element.height(parseInt(height));
}

/************* Load view *************/

function LoadView(container, parent)
{
    AbstractView.call(this, container, parent);
    this.element.attr("id", "load");
    var $loader =  $('<div class="loader"></div>');
    var $progressBar =  $('<div class="progress-bar"><div class="progress"></div></div>');
    var $text = $('<div class="text"></div>');
    $loader.append([$progressBar, $text]);
    this.element.append($loader);
    $loader.css("top", parseInt(this.element.height()/2 - $loader.height()/2) + "px");
    this.progress = 0;
    this.total = 0;
}

belle.extend(LoadView, AbstractView);

LoadView.prototype.update = function(data)
{
  this.progress = data.loaded;
  this.total = data.total;
  this.render();
}

LoadView.prototype.render = function()
{
  var loader = this.element.find(".loader");
  //default animation
  if (this.progress == 0 && this.total == 0) {
    var self = this,
      progress = loader.find(".progress");
      progress.animate( {
        "left" : loader.width() + 2,
        "easing" : "linear"
      },
      2000,
      function(){
        progress.css("left", -progress.width());
        setTimeout(function() {
          self.render();
        }, 20);
      }
    );
    return;
  }

  loader.find(".progress").stop().css({
    "left" : 0,
    "width" : parseInt((loader.width() * this.progress) / this.total)
  });

  var percent = Math.round((this.progress * 100) / this.total);
  this.element.find(".text").html("Loading assets " + "(" + percent + "%)");
}

/************* Game canvas view *************/

function GameView(container, parent)
{
    AbstractView.call(this, container, parent);
    this.clearObjects = [];
    this.drawing = false;
    this.bgcanvas = $('<canvas class="background"></canvas>');
    this.canvas = $('<canvas></canvas>');
    this.element.append([this.bgcanvas, this.canvas]);
    this.bgcontext = this.bgcanvas[0].getContext('2d');
    this.context = this.canvas[0].getContext('2d');
    this.pressedObject = this.hoveredObject = null;
    this._cleared = false;
    this._actionToSkip = null;
}

belle.extend(GameView, AbstractView);

GameView.prototype.setModel = function(model)
{
  AbstractView.prototype.setModel.call(this, model);

  this.setSize(this.model.properties.width, this.model.properties.height);

  this.model.bind("sceneChanged", this, function() {
    this.reset();
  });

  this.model.bind("objectRemoved", this, function(data) {
    if (data && data.object) {
      if (data.object == this.hoveredObject)
        this.hoveredObject = null;
      if (data.object == this.pressedObject)
        this.pressedObject = null;
    }
    this.clear();
  });

  this.model.bind("resumed", this, function(data) {
    this.render();
  });
}

GameView.prototype.clear = function(all)
{
  if (all)
    this.bgcontext.clearRect(0, 0, this.bgcanvas.attr("width"), this.bgcanvas.attr("height"));
  this.context.clearRect(0, 0, this.canvas.attr("width"), this.canvas.attr("height"));
  this._cleared = true;
}

GameView.prototype.reset = function()
{
  this.pressedObject = this.hoveredObject = null;
  this.clear(true);
}

GameView.prototype.setWidth = function(width)
{
  AbstractView.prototype.setWidth.call(this, width);
  this.bgcanvas.attr("width", width);
  this.canvas.attr("width", width);
}

GameView.prototype.setHeight = function(height)
{
  AbstractView.prototype.setHeight.call(this, height);
  this.bgcanvas.attr("height", height);
  this.canvas.attr("height", height);
}

GameView.prototype.mapToScene = function(event) {
  if (! this.contains(event.pageX, event.pageY))
    return false;

  var offset = this.element.offset(),
      width = this.element.width(),
      height = this.element.height(),
      x = event.pageX,
      y = event.pageY;

  x -= offset.left;
  y -= offset.top;
  event.canvasX = Math.round(x / this.scaledWidthFactor);
  event.canvasY = Math.round(y / this.scaledHeightFactor);
  return true;
}

GameView.prototype._detectHoveredObject = function(ev)
{
    var scene = this.model.getScene();
    if (! scene)
      return;

    var objects = scene.objects;
    for (var i=objects.length-1; i !== -1; --i) {
        if (objects[i].contains(ev.canvasX, ev.canvasY)) {
            return objects[i];
        }
    }

    return null;
}

GameView.prototype.mouseMove = function(ev) {
  var game = this.model ? this.model.getGame() : null;
  var prevHoveredObject = this.hoveredObject;
  var mapped = this.mapToScene(ev);

  if (mapped)
    this.hoveredObject = this._detectHoveredObject(ev);
  else
    this.hoveredObject = null;

  if (prevHoveredObject != this.hoveredObject) {
    if (prevHoveredObject) {
      prevHoveredObject.mouseLeaveEvent(ev);
      if (game)
        game.removeWatchedObject(prevHoveredObject);
    }

    if (this.hoveredObject) {
      if (game)
        game.addWatchedObject(this.hoveredObject);
      this.hoveredObject.mouseEnterEvent(ev);
    }
  }
  else if (this.hoveredObject)
    this.hoveredObject.mouseMove(ev);
}

GameView.prototype.mouseDown = function(ev) {
  if (ev.type == "mousedown" && this._detectButton(ev) != "left")
    return;

  this.pressedObject = this.hoveredObject;

  //there may be no hovered object on touch devices
  if (! this.pressedObject && ev.type == "touchstart") {
      this.mapToScene(ev);
      this.pressedObject = this.hoveredObject = this._detectHoveredObject(ev);
      this.hoveredObject.mouseEnterEvent(ev);
      this.hoveredObject.mouseDown(ev);
  }
  else if (this.hoveredObject) {
      this.mapToScene(ev);
      this.hoveredObject.mouseDown(ev);
  }
}

GameView.prototype.mouseUp = function(ev) {
    if (ev.type == "mouseup" && this._detectButton(ev) != "left")
      return;

    if (! this.mapToScene(ev))
        return;

    if (this.pressedObject && this.hoveredObject == this.pressedObject) {
        this.hoveredObject.mouseUp(ev);
        this.pressedObject = null;
        if (ev.type == "touchend") {
          this.hoveredObject.mouseLeaveEvent(ev);
          this.hoveredObject = null;
        }
    }

    if (! ev.processed) {
      var a = this.model.getAction();
      if (a)
        a.skip();
    }
}

GameView.prototype.keyDown = function (ev)
{
    if (!this.model)
      return;

    switch(ev.keyCode) {
        case 13: //ENTER
        case 32: //SPACE
            this._actionToSkip = this.model.getAction();
    }
}

GameView.prototype.keyUp = function (ev)
{
    if (!this.model)
      return;

    switch(ev.keyCode) {
        case 13: //ENTER
        case 32: //SPACE
            var a = this.model.getAction();
            if (a && a == this._actionToSkip)
                a.skip();
            this._actionToSkip = null;
    }
}

GameView.prototype._render = function () {
    if (this.drawing || !this.model || ! this.model.scene)
      return;

    this.drawing = true;
    var scene = this.model.getScene();
    var objects = scene.getObjects();
    var obj = null;
    var i, j, redraw=false;
    var font = this.model.properties.font;

    if (font)
      this.context.font = font;
    
    if (this._cleared) {
      scene.paint(this.bgcontext);

      for(j=0; j != objects.length; j++) {
          obj = objects[j];
          this.context.save();
          obj.redraw = true;
          obj.paint(this.context);
          obj.redraw = false;
          this.context.restore();
      }
      this._cleared = false;
    }
    else {
      if (scene.redraw) {
        scene.paint(this.bgcontext);
        scene.redraw = false;
      }

      var redraws = [];

      for(i=0; i < objects.length; i++) {
        redraws[i] = objects[i].redraw;
      }

      for(i=objects.length-1; i !== -1; --i) {
          if (redraws[i]) {
            obj = objects[i];
            redraw = true;
            for(j=objects.length-1; j !== -1; --j) {
                if (redraws[j] || i == j)
                    continue;

                if (obj.isPainted() && obj.overlaped(objects[j])) {
                  redraws[j] = true;
                }
                else if (obj.overlaps(objects[j])) {
                  redraws[j] = true;
                }
            }
          }
      }

      if (redraw) {
        for(j=0; j !== objects.length; j++)
            if (redraws[j])
              objects[j].clear(this.context);

        for(j=0; j !== objects.length; j++) {
            if (redraws[j]) {
              obj = objects[j];
              this.context.save();
              obj.paint(this.context);
              obj.redraw = false;
              this.context.restore();
            }
        }
      }
    }

    this.drawing = false;
}

GameView.prototype.render = function () {
  if (!this.model || this.model.isPaused() || this.model.isFinished()) {
    this._render(); //do last render
    return;
  }

  var self = this;
  window.requestAnimationFrame(function(){
    self.render.call(self);
  });

  this._render();
}

belle.views.AbstractView = AbstractView;
belle.views.GameView = GameView;
belle.views.LoadView = LoadView;

}(belle));
