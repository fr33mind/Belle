belle = {};
game = {};
belle.log = function() {
  if(window.console)
    window.console.log( Array.prototype.slice.call(arguments) );
};
belle.actions = {};
belle.objects = {};
belle.utils = {};
belle.core = {};
belle.views = {};
belle.display = {};
belle.graphics = {};
belle.game = {
  directory: ""
};
function surrogateCtor() {}

belle.extend = function (sub, base)
{
    // Copy the prototype from the base to setup inheritance
    surrogateCtor.prototype = base.prototype;
    // Tricky huh?
    sub.prototype = new surrogateCtor();
    // Remember the constructor property was set wrong, let's fix it
    sub.prototype.constructor = sub;
}

belle.isInstance = function(object, constructor) {

  while (object != null) {
    if (object == constructor.prototype)
      return true;
    object = Object.getPrototypeOf(object);
  }
  return false;
}

belle.serialize = function(obj)
{
  var prop;

  if (! obj)
    return null;

  if (typeof obj == "number" || typeof obj == "string" || typeof value == "boolean")
    return obj;
  else if (obj instanceof Array) {
    prop = [];
    for(var i=0; i < obj.length; i++)
      prop.push(belle.serialize(obj[i]));
  }
  else if (typeof obj == "object") {
    prop = {};
    //if derived from Belle's Object, use its serialize function
    if (typeof obj.serialize == "function"){
      prop = obj.serialize();
    }
    else {
      for(key in obj) {
        if (obj.hasOwnProperty(key)) {
          prop[key] = belle.serialize(obj[key]);
        }
      }
    }
  }

  return prop;
}

belle.createObject = function(data, parent)
{
    var type = data["type"];
    if (! type) {
        belle.log("Object has no type");
        return null;
    }

    var ObjectClass = belle.objects[type];

    if (! ObjectClass) {
        belle.log("'" + type + "' is not a valid object type.");
        return null;
    }


    var obj = new ObjectClass(data, parent);

    return obj;
}

belle.createAction = function(data, parent)
{
    var type = data["type"];
    var _Action = belle.actions[type];

    if (! _Action) {
        belle.log("'" + type + "' is not a valid action type.");
        return null;
    }

    return new _Action(data, parent);
}

belle.isNumber = function(num)
{
  num = parseInt(num);
  return (num === num);
}

//set requestAnimationFrame
window.requestAnimationFrame = (function(){
  return  window.requestAnimationFrame       ||
          window.webkitRequestAnimationFrame ||
          window.mozRequestAnimationFrame    ||
          window.oRequestAnimationFrame      ||
          window.msRequestAnimationFrame     ||
          function( callback ){
            window.setTimeout(callback, 1000 / 60);
          };
})();

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

