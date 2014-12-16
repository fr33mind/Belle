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

function mapToDisplay(event)
{
    if (! container.length)
        container = $("#belle");
    var x;
    var y;
    var IE = document.all ? true : false; // check to see if using IE

    if (IE) { //do if internet explorer 
        var scrollLeft = document.documentElement ? document.documentElement.scrollLeft :document.body.scrollLeft;
        x = event.clientX + scrollLeft;
        var scrollTop = document.documentElement ? document.documentElement.scrollTop :document.body.scrollTop;
        y = event.clientY + scrollTop;
    }
    else {  //do for all other browsers
        x = (window.Event) ? event.pageX : event.clientX + (document.documentElement.scrollLeft ? document.documentElement.scrollLeft : document.body.scrollLeft);
        y = (window.Event) ? event.pageY : event.clientY + (document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop);
    }

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

document.onmousemove = function(event)
{
    var scene = game.getScene();
    if (! scene)
      return;
    
    var ev = event || window.event || window.Event;
    if (! mapToDisplay(ev))
        return;
    
    var prevHoveredObject = hoveredObject;
    hoveredObject = null;
    var objects = scene.objects;
    for (var i=objects.length-1; i !== -1; --i) {
        if (objects[i].contains(ev.canvasX, ev.canvasY)) {
            hoveredObject = objects[i];
            break;
        }
    }

    if (prevHoveredObject != hoveredObject) {
      if (prevHoveredObject)
	prevHoveredObject.mouseLeaveEvent(ev);
      
      if (hoveredObject)
	hoveredObject.mouseEnterEvent(ev);
    }
    else if (hoveredObject)
      hoveredObject.processEvent(ev);
}

document.onmouseup = function(event)
{
    var ev = event || window.event || window.Event;
    var processed = false;
    
    if (detectbutton(ev) != "left")
      return;
    
    if (! mapToDisplay(ev))
        return;
    
    if (pressedObject && hoveredObject == pressedObject) {
        if (hoveredObject.processEvent(ev))
            processed = true;
    }
    
    if (! processed) {
      var a = game.getScene().getAction();
      if (a)
	a.skip();
    }
}

document.onmousedown = function(event) 
{
    var ev = event || window.event || window.Event;
    
    if (detectbutton(ev) != "left")
      return;
      
    pressedObject = hoveredObject;
    
    if (hoveredObject) {
        mapToDisplay(event);
        hoveredObject.processEvent(ev);
    }
}

document.onkeyup = function(event) 
{
    var ev = event || window.event || window.Event;

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
}

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

}(EventDispatcher));

belle.log("Event module loaded!");
