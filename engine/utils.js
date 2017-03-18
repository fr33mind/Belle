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
 
(function(utils) {

var dummy = null;
 
function getBody()
{
    var body = document.getElementById("body");
    if (! body)
        body = document.body;
    if (! body)
        body = document.getElementById("body")[0];
    
    return body;
}

var parseSize = function(value)
{
    if (parseInt(value) != NaN)
        return parseInt(value);
    
    if (utils.isPercentSize(value)) {
        value.split("%")
        var parts = value.split("%");
        if (parts && parseInt(parts[0]) != -1)
            return parseInt(value);
    }

    return 0;
}

utils.isObjectEmpty = function(object)
{
    if (Object.getOwnPropertyNames) {
        return !(Object.getOwnPropertyNames(object).length);
    }
    else {
        for(var property in object) 
            if(object.hasOwnProperty(property))
                return false;
    }
    return true;
}

utils.textSize = function(text, font) 
{
    var size = [];
    if (! dummy)
      dummy = $('#dummy');

    dummy.css('font', font);
    dummy.html(text);
    
    size.push(dummy.width());
    size.push(dummy.height());

    return size;
}

utils.textWidth = function(text, font) 
{
    if (! font)
        return 0;
    
    if (! dummy)
      dummy = document.getElementById("dummy");
    dummy.style.font = font;
    dummy.innerHTML = text;
    return dummy.offsetWidth;
}

utils.splitText = function(text, font, maxWidth) {
     if (! text)
        return [""];
     
    var dummy = document.getElementById("dummy");
    dummy.style.font = font;
    dummy.innerHTML = text;
    var textSplit = [];

    if (text.contains("\n")) {
      textSplit = text.split("\n");
      var resultText = [],
          subTextSplit;

      for(var i=0; i < textSplit.length; i++) {
          subTextSplit = utils.splitText(textSplit[i], font, maxWidth);
          for(var j=0; j < subTextSplit.length; j++) {
            resultText.push(subTextSplit[j]);
          }
      }

      return resultText;
    }
    
    if (dummy.offsetWidth > maxWidth) {
      var words = text.split(" ");
        
      var line="";
      for(var i=0; i < words.length; i++) {
          word = words[i];
          dummy.innerHTML = line.length ? line + " " + word : word;;
          if (dummy.offsetWidth > maxWidth) {
            line = line.length ? line : word;
            textSplit.push(line);
            line = "";
          }
          line = line.length ? line + " " + word : word;
      }
        
      if (line.length) {
        textSplit.push(line);
      }
      
      return textSplit;
    }
    
    return [text];   
}

utils.extendJsonObject = function (a, b)
{
    for(var key in b)
        if(! a.hasOwnProperty(key))
            a[key] = b[key];
}

utils.isPercentSize = function(value)
{
    if (typeof value != "string")
        return false;

    return value.search(/[0-9]+\%/g) != -1;
}

utils.isNaN = function(value)
{
  //NaN value is never equal to itself
  return (value !== value);
}

utils.isNumber = function(n) {
  return ! utils.isNaN(parseInt(n));
}

utils.initElement = function (element, info)
{
    element.style.position = "absolute";
    if (typeof info.width == "string")
        element.style.width = info.width;
    else
        element.style.width = info.width + "px";
    
    if (typeof info.height == "string")
        element.style.height = info.height;
    else
        element.style.height = info.height + "px";
    element.style.display = "none";
    element.style.filter = "inherit";
}

utils.importFile = function(url, callback, async, mimeType)
{
    if (async == "undefined")
        async = false;
    
    var xobj = new XMLHttpRequest();
    if (xobj.overrideMimeType && mimeType)
        xobj.overrideMimeType(mimeType);
    
    xobj.onreadystatechange = function() {
        if(xobj.readyState == 4){
            if (callback)
                callback(xobj);
        }
    };
    
    xobj.open("GET", url, async);
    xobj.send(null);
}

utils.isFontLoaded = function(name)
{
    name = utils.getFontName(name);

    var defaultFont = "12px Arial, Helvetica, sans-serif";
    var font = "12px " + name + ",  Arial, Helvetica, sans-serif";
    var defaultWidth = utils.textWidth("ABCDEFGHIJKLMNOPQRSTUVXWYZ0123456789", defaultFont);
    var width = utils.textWidth("ABCDEFGHIJKLMNOPQRSTUVXWYZ0123456789", font);
    if ( width != defaultWidth)
        return true;
    return false;
}

utils.isFontAvailable = function(fontFile)
{
    var status = 404;
    utils.importFile(fontFile, function(obj) {status = obj.status;}, false, "application/octet-stream");
    if (status == 200)
        return true;
    return false;
}

utils.getFontName = function(font)
{
    if (font.indexOf(".") !== -1)
        return font.split(".")[0];
    if (font.indexOf("px") !== -1)
        return font.split("px")[1];
    if (font.indexOf("%") !== -1)
        return font.split("%")[1];
    
    return font;
}

utils.getNewName = function(name) {
    
    var re = /^.*\([ ]*([0-9]+)[ ]*\)[ ]*$/i;
    
    for (var i=name.length; i >= 0; --i) {
        if (parseInt(name[i]) === NaN)
            break;
    }
    
    var matches = name.match(re);
    if (matches.length >= 2) {	
		var start = name.lastIndexOf("(");
		var end = name.lastIndexOf(")");
		var num = matches[1];
		if (start != -1 && end != -1) {
			name = name.substring(0, start+1) + num + name.substring(end, name.length);  
		}
    }
    else {
		name = name + " (2)";
	}
      
    return name;
}

utils.getSaveDate = function()
{
    var date = new Date();
    var hour = date.getHours();
    var min = date.getMinutes();
    if (hour < 10)
        hour = '0' + hour;
    if (min < 10)
        min = '0' + min;
    
    date = date.toDateString() + ", " + hour + ":" + min;
    return date;
}

utils.setBorderRadius = function(elem, radius) {
    elem.style.borderRadius = radius + "px"; // standard
    elem.style.MozBorderRadius = radius + "px"; // Mozilla
    elem.style.WebkitBorderRadius = radius + "px";; // WebKit
}

utils.distance = function(x1, y1, x2, y2)
{
  return Math.sqrt(Math.pow(x2-x1, 2) + Math.pow(y2-y1, 2));
}

}(belle.utils));
