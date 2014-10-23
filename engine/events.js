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

var EventDispatcher = {};

(function(EventDispatcher) {

var container = $("#belle");
var hoveredObject = null;
var pressedObject = null;
var display = belle.display;
var touchEnabled = false;

function resize() 
{
    var _view = 'portrait';
    if (display.windowWidth() > display.windowHeight())
        _view = 'landscape';
        
    if (display.view != _view) {
        display.view = _view;    
        display.resize();
    }
}

if (window.addEventListener) 
  window.addEventListener('resize', resize, false);
else if (window.attachEvent)
  window.attachEvent('resize', resize);

function detectbutton(e)
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

function detectHoveredObject(ev)
{
    var scene = game.getScene();
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

function mapToDisplay(event)
{
    if (! container.length)
        container = $("#belle");

    var x = event.pageX;
    var y = event.pageY;
    var offset = container.offset();
    var width = container.width();
    var height = container.height();

    x -= offset.left;
    y -= offset.top;
    event.canvasX = x;
    event.canvasY = y;

    if (x < 0 || y < 0 || x > width || y > height)
        return false;
    
    return true;
}

function mouseDown(ev)
{
    if (ev.type == "mousedown" && detectbutton(ev) != "left")
      return;

    pressedObject = hoveredObject;

    //there may be no hovered object on touch devices
    if (! pressedObject && ev.type == "touchstart") {
        mapToDisplay(ev);
        pressedObject = hoveredObject = detectHoveredObject(ev);
        hoveredObject.mouseEnterEvent(ev);
        hoveredObject.mouseDown(ev);
    }
    else if (hoveredObject) {
        mapToDisplay(ev);
        hoveredObject.mouseDown(ev);
    }
}

function mouseUp(ev)
{
    if (ev.type == "mouseup" && detectbutton(ev) != "left")
      return;

    var processed = false;
    if (! mapToDisplay(ev))
        return;

    if (pressedObject && hoveredObject == pressedObject) {
        if (hoveredObject.mouseUp(ev))
            processed = true;
        pressedObject = null;
        if (ev.type == "touchend") {
          hoveredObject.mouseLeaveEvent(ev);
          hoveredObject = null;
        }
    }

    if (! processed) {
      var a = game.getScene().getAction();
      if (a)
        a.skip();
    }
}

function mouseMove(ev)
{
    if (! mapToDisplay(ev))
        return;

    var prevHoveredObject = hoveredObject;
    hoveredObject = detectHoveredObject(ev);

    if (prevHoveredObject != hoveredObject) {
      if (prevHoveredObject)
        prevHoveredObject.mouseLeaveEvent(ev);

      if (hoveredObject)
        hoveredObject.mouseEnterEvent(ev);
    }
    else if (hoveredObject)
      hoveredObject.mouseMove(ev);
}

//equivalent to mousedown
$(document).bind('touchstart', function(e){
  var touch = e.originalEvent.touches[0] || e.originalEvent.changedTouches[0];
  if (touch) {
    touchEnabled = true;
    e.preventDefault();
    e.stopPropagation();
    e.pageX = touch.pageX;
    e.pageY = touch.pageY;
    mouseDown.call(this, e);
  }
});

//equivalent to mouseup
$(document).bind('touchend', function(e){
  var touch = e.originalEvent.touches[0] || e.originalEvent.changedTouches[0];
  if (touch) {
    touchEnabled = true;
    e.preventDefault();
    e.stopPropagation();
    e.pageX = touch.pageX;
    e.pageY = touch.pageY;
    mouseUp.call(this, e);
  }
});

//equivalent to mousemove
$(document).bind('touchmove', function(e){
  var touch = e.originalEvent.touches[0] || e.originalEvent.changedTouches[0];
  if (touch) {
    touchEnabled = true;
    e.preventDefault();
    e.stopPropagation();
    e.pageX = touch.pageX;
    e.pageY = touch.pageY;
    mouseMove.call(this, e);
  }
});

$(document).bind('mousedown', function(e){
  if (touchEnabled)
    return;
  mouseDown.call(this, e);
});

$(document).bind('mouseup', function(e){
  if (touchEnabled)
    return;
  mouseUp.call(this, e);
});

$(document).bind('mousemove', function(e){
  if (touchEnabled)
    return;
  mouseMove.call(this, e);
});

$(document).keyup(function(ev) {
  switch(ev.keyCode) {
        case 13: //ENTER
        case 32: //SPACE
	    var a = game.getScene().getAction();
            if (a)
                a.skip();
        break;
        case 27:
            belle.pause();
            break;
    }
});

}(EventDispatcher));

belle.log("Event module loaded!");
