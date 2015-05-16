(function(belle){

  function Object(parent)
  {
    this.events = {};
    this.parent = parent;
  }

  Object.prototype.trigger = function(event, data) {
    var calls = this.events[event] || [],
        call = null;
        
    if (! calls.length)
      return false;

    data = data ? data : {};

    for(var i=0; i < calls.length; i++) {
      call = calls[i];
      call.callback.call(call.caller, data);
    }

    return true;
  }

  Object.prototype.bind = function(event, arg2, arg3, arg4) {
    var caller = this,
        callback = null,
        once = (arg4 === true);

    if (typeof arg2 == "object") {
      caller = arg2;
      callback = arg3;
    }
    else {
      callback = arg2;
    }

    if (typeof event != "string" || typeof callback != "function")
      return false;

    if (once && this.hasEvents(event, caller))
      return true;

    var self = this,
        listener = {
          caller: caller,
          callback: callback
        },
        calls = this.events[event] || [];

    calls.push(listener);
    this.events[event] = calls;
    return true;
  }

  Object.prototype.getEvents = function(event, arg2, arg3) {
    var caller = null,
        callback = null,
        calls = this.events[event] || [],
        call = null,
        events = [];

    if (! calls.length)
      return events;

    if (typeof arg2 == "object") {
      caller = arg2;
      callback = arg3;
    }
    else {
      callback = arg2;
    }

    for (var i=calls.length-1; i >= 0; --i) {
      call = calls[i];
      if ((!caller || call.caller == caller) && (!callback || callback == call.callback)) {
        events.push(call);
      }
    }

    return events;
  }

  Object.prototype.hasEvents = function(event, arg2, arg3) {
    return this.getEvents(event, arg2, arg3).length;
  }

  Object.prototype.unbind = function(event, arg2, arg3) {
    var i = 0,
        success = false,
        unbindEvents = this.getEvents(event, arg2, arg3),
        events = this.events[event];

    if (! unbindEvents.length)
      return false;

    for(i=events.length-1; i >= 0; --i)  {
      if (unbindEvents.indexOf(events[i]) !== -1)
        events.splice(i, 1);
    }

    return true;
  }

  /**** Point ****/

  function Point(x, y)
  {
    this.x = x;
    this.y = y;

    if (this.x === null || this.x === undefined)
        this.x = 0;
    if (this.y === null || this.y === undefined)
        this.y = 0;
  }

  Point.prototype.distance = function(point)
  {
    return Math.sqrt(Math.pow(point.x-this.x, 2) + Math.pow(point.y-this.y, 2));
  }

  /*** Asset ***/

  function Asset(path)
  {
    Object.call(this);

    this.path = path;
    this.name = "";
    if (this.path) {
      var parts = path.split("/");
      this.name = parts[parts.length-1];
    }
  }

  belle.extend(Asset, Object);

  Asset.prototype.getPath = function()
  {
    return this.path;
  }

  Asset.prototype.getName = function()
  {
    return this.name;
  }

  /*** Timer - Wrapper object for setTimeout/setInterval ***/

  function Timer(callback, timeout, repeat)
  {
    this.timerId = -1;
    this.callback = callback;
    this.timeout = timeout || 0;
    this.repeat = repeat || false;
    this.startTime = null;
    this.stopTime = null;
    this._lastCallTime = null;
    this._repeat = null;
  }

  Timer.prototype.isActive = function()
  {
    if (this.timerId != -1)
      return true;
    return false;
  }

  Timer.prototype.start = function()
  {
    if (typeof this.callback != "function")
      return null;

    //pass this timer object to callback
    var callback = function() {
      this.callback(this);
      if (! this._repeat)
        this.onStop();
      else
        this._lastCallTime = new Date().getTime();
    }.bind(this);

    this.onStart();

    this._repeat = this.repeat;

    if (this._repeat)
      this.timerId = window.setInterval(callback, this.timeout);
    else
      this.timerId = window.setTimeout(callback, this.timeout);
  }

  Timer.prototype.stop = function()
  {
    if (this.timerId == -1)
      return;

    if (this._repeat)
      window.clearInterval(this.timerId);
    else
      window.clearTimeout(this.timerId);

    this.onStop();
  }

  Timer.prototype.onStart = function()
  {
    if (this.isActive())
      this.stop();

    this.startTime = new Date().getTime();
    this._lastCallTime = this.startTime;
    this.stopTime = null;
  }

  Timer.prototype.onStop = function()
  {
    this.timerId = -1;
    this.stopTime = new Date().getTime();
  }

  Timer.prototype.getElapsedTime = function()
  {
    if (! this.startTime)
      return 0;

    if (this.stopTime !== null)
      return (this.stopTime - this.startTime);

    var now = new Date().getTime();
    return (now - this.startTime);
  }

  Timer.prototype.getDeltaTime = function()
  {
    if (! this._lastCallTime)
        return 0;

    if (this.stopTime !== null)
      return (this.stopTime - this._lastCallTime);

    var now = new Date().getTime();
    return now - this._lastCallTime;
  }

  belle.core.Object = Object;
  belle.core.Point = Point;
  belle.core.Asset = Asset;
  belle.core.Timer = Timer;

}(belle));
