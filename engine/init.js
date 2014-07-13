belle = {};
belle.actions = {};
belle.objects = {};
belle.display = {};
belle.utils = {};
belle.stateMachine = {};

game = {
    "resources" : {},
    "customFonts" : [],
    "scenes" : [],
    "scene": null,
    "ready" : false,
    "width": 640,
    "height" : 480,
    "currentWidth": 640,
    "currentHeight": 480,
    "variables" : {},
    "finished" : false,
    "textSpeed" : 10,
    "textDelay" : 100,
    "filename" : "game.json",
    "directory" : "",
    "states" : {},
    "data" : null,
    "paused": false,
    "eventListeners" : {
      "variableChanged" : []
    },
    "soundManager": {}
};

game.pauseScreen = {
    "scenes" : [],
    "scene": null,
    "states" : {},
    "finished": false
};


//Extend Array and String objects
if (typeof String.prototype.startsWith != 'function') 
{
  String.prototype.startsWith = function (str) {
    return this.slice(0, str.length) == str;
  };
}

if (typeof Array.prototype.indexOf != 'function')
{
  Array.prototype.indexOf = function(value, start)
  {
    if (! start)
        start = 0;
    
    for (var i=start; i < this.length; i++)
        if (this[i] === value)
            return i;
   
    return -1;
  };
}

if (typeof String.prototype.contains != 'function') 
{
    String.prototype.contains = function(text) { 
        return this.indexOf(text) != -1; 
    };
}

if (typeof String.prototype.trim != 'function') 
{
    String.prototype.trim =  function(){
        return this.replace(/^\s+|\s+$/g, '');
        
    };
}

if (typeof String.prototype.isDigit != 'function') 
{
    String.prototype.isDigit = function(text) { 
        text = text.trim();
        return ! isNaN(text); 
    };
}

if (typeof String.prototype.hasBreakPointAt != 'function') 
{
    String.prototype.hasBreakPointAt = function(i) { 
        return this[i] == " " || this[i] == "\n"; 
    };
}

if (typeof Array.prototype.contains != 'function') 
{
    Array.prototype.contains = function(text) { 
        return this.indexOf(text) != -1; 
    };
}

