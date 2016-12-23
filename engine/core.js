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

  var ConditionTokenTypes = {
    Unknown: 0,
    Variable: 1,
    Value: 2,
    String: 2,
    Number: 3,
    Boolean: 4,
    LogicalOperator: 16,
    AbstractCondition: 17,
    SimpleCondition: 18,
    ComplexCondition: 19,
    LiteralCondition: 20
  };

  var ConditionTokenFactory = {
    createToken: function(data) {
      var std_data = ConditionTokenFactory.standardData(data);

      switch(std_data.type) {
        case ConditionTokenTypes.Variable:
        case ConditionTokenTypes.Value:
        case ConditionTokenTypes.Number:
        case ConditionTokenTypes.Boolean:
        case ConditionTokenTypes.LogicalOperator:
          return new ConditionToken(std_data);
        case ConditionTokenTypes.SimpleCondition:
        case ConditionTokenTypes.ComplexCondition:
        case ConditionTokenTypes.LiteralCondition:
          return ConditionTokenFactory.createCondition(std_data);
      }

      return null;
    },
    createCondition: function(data) {
      var std_data = ConditionTokenFactory.standardData(data, true);
      switch(std_data.type) {
        case ConditionTokenTypes.SimpleCondition:
          return new SimpleCondition(std_data);
        case ConditionTokenTypes.ComplexCondition:
          return new ComplexCondition(std_data);
        case ConditionTokenTypes.LiteralCondition:
          return new LiteralCondition(std_data);
      }

      return null;
    },
    standardData: function(data, condition) {
      var _data = {};
      if (typeof data == 'object') {
        if (belle.isNumber(data.type)) {
          _data = data;
        }
        else {
          _data.type = ConditionTokenTypes.Unknown;
        }
      }
      else if (typeof data == 'string') {
        if (condition)
          _data.type = ConditionTokenTypes.LiteralCondition;
        else
          _data.type = ConditionTokenTypes.Value;
        _data.value = data;
      }

      return _data;
    }
  };

  var ConditionOperationTypes = {
    Unknown: 0,
    Equal: 1,
    NotEqual: 2,
    Contains: 3,
    GreaterThan: 4,
    GreaterThanOrEqual: 5,
    LesserThan: 6,
    LesserThanOrEqual: 7,
    IsTrue: 8,
    IsFalse: 9,
    IsDefined: 10,
    IsUndefined: 11
  };

  var ConditionLogicalOperatorTypes = {
    And: 0,
    Or: 1
  };

  var ConditionLogicalOperator = {
    toString: function(type) {
      switch(type) {
        case ConditionLogicalOperatorTypes.And: return "&&";
        case ConditionLogicalOperatorTypes.Or: return "||";
        default: return "";
      }
    }
  }

  var ConditionToken = function(data) {
    var _data = data;
    if (!jQuery.isPlainObject(_data)) {
      _data = {};
      _data.value = data;
    }
    this.type = _data.type || ConditionTokenTypes.Unknown;
    this.value = _data.value;
  }

  ConditionToken.prototype.toString = function() {
    switch(this.type) {
      case ConditionTokenTypes.LogicalOperator:
        return ConditionLogicalOperator.toString(this.value);
      case ConditionTokenTypes.Value:
      case ConditionTokenTypes.String:
        return '"' + this.value + '"';

    }

    return this.value;
  }

  var AbstractCondition = function(data) {
    ConditionToken.call(this, data);
    this.type = ConditionTokenTypes.AbstractCondition;
  }

  belle.extend(AbstractCondition, ConditionToken);

  AbstractCondition.prototype.eval = function(variables) {
    var result = null,
        condition = this.toString();
    variables = variables || {};

    (function() {
      var vars_text = "";
      for (var name in variables) {
        vars_text += 'var ' + name + ' = variables["' + name + '"];';
      }
      
      try {
        eval(vars_text);
        result = eval(condition);
      }
      catch(ex) {
        result = false;
      }
    }());

    return result;
  }

  var SimpleCondition = function(data) {
    AbstractCondition.call(this, data);
    this.type = ConditionTokenTypes.SimpleCondition;
    this.leftOperand = ConditionTokenFactory.createToken(this.value.leftOperand);
    this.operation = this.value.operation;
    this.rightOperand = this.value.rightOperand !== undefined ? ConditionTokenFactory.createToken(this.value.rightOperand) : null;
  }

  belle.extend(SimpleCondition, AbstractCondition);

  SimpleCondition.prototype.toString = function() {
    var types = ConditionOperationTypes,
        leftStr = this.leftOperand && belle.isInstance(this.leftOperand, ConditionToken) ? this.leftOperand.toString() : 'null',
        rightStr = this.rightOperand && belle.isInstance(this.rightOperand, ConditionToken) ? this.rightOperand.toString(): 'null';

    switch(this.operation) {
      case types.Equal:
        return leftStr + ' == ' + rightStr;
      case types.NotEqual:
        return leftStr + ' != ' + rightStr;
      case types.Contains:
        return leftStr + '.indexOf('+rightStr+') != -1';
      case types.GreaterThan:
        return 'Number(' + leftStr + ') > Number(' + rightStr + ')';
      case types.GreaterThanOrEqual:
        return 'Number(' + leftStr + ') >= Number(' + rightStr + ')';
      case types.LesserThan:
        return 'Number(' + leftStr + ') < Number(' + rightStr + ')';
      case types.LesserThanOrEqual:
        return 'Number(' + leftStr + ') <= Number(' + rightStr + ')';
      case types.IsTrue:
        return '(!!' + leftStr + ') == true';
      case types.IsFalse:
        return '(!!' + leftStr + ') == false';
      case types.IsDefined:
        return leftStr + ' !== undefined';
      case types.IsUndefined:
        return leftStr + ' === undefined';
    }

    return "";
  }

  var LiteralCondition = function(data) {
    AbstractCondition.call(this, data);
    this.type = ConditionTokenTypes.LiteralCondition;
    var condition = LiteralCondition.translate(this.value);
    if (condition)
      this.value = condition;
  };

  belle.extend(LiteralCondition, AbstractCondition);

  LiteralCondition.translate = function(condition)
  {
    var symbol = "";
    var name = "";
    var _in = "in";
    var _and = "and";
    var _or = "or";
    var validchar = /^[a-zA-Z_]$/g;
    var number = /^[0-9]|([0-9]*\.[0-9]+)$/g;
    var variable_pattern = /^\[a-zA-Z_]+[a-zA-Z_0-9]*$/;
    var c = "";
    var i = 0;
    var string = false;
    var string_del = '';
    var curr_condition = [];
    var lop = "";
    var final_condition = "";

    function translateCondition(lop, tokens)
    {
      var condition = tokens.join(' ');
      if (tokens.length == 3) {
        if (tokens[1] == "in") {
          condition = tokens[2] + ".indexOf(" + tokens[0] + ") != -1";
        }
      }

      if (lop == 'and')
        lop = '&&';
      else if (lop == 'or')
        lop = '||';

      tokens.length = 0;
      return ' ' + lop + ' ' + condition;
    }

    for(i=0; i < condition.length; i++) {
        c = condition[i];

        if (c == '"' || c == "'") {
            string = string && c == string_del ? false : true;
            if (string) {
              if (!string_del)
                string_del = c;
            }
            else
              string_del = '';
        }

        if (string) {
            name += c;
            continue;
        }

        if (c.search(validchar) != -1 || c.search(number) != -1 || c == '"' || c == "'" || c == '.') {
            if (! name) {
                if (condition.substr(i, _in.length+1).toLowerCase().trim() == _in) {
                    i += _in.length - 1;
                    curr_condition.push(_in);
                }
                else if (condition.substr(i, _and.length+1).toLowerCase().trim() == _and) {
                    i += _and.length - 1;
                    final_condition += translateCondition(lop, curr_condition);
                    lop = _and;
                }
                else if (condition.substr(i, _or.length+1).toLowerCase().trim() == _or) {
                    i += _or.length - 1;
                    final_condition += translateCondition(lop, curr_condition);
                    lop = _or;
                }
                else
                    name += c;
            }
            else
                name += c;
        }
        else {
            if (name) {
                curr_condition.push(name);
                name = "";
            }

            if (c == '&' || c == '|') {
                symbol = condition.substr(i, 2);
                if (symbol == c + c) {
                    final_condition += translateCondition(lop, curr_condition);
                    lop = symbol;
                    ++i;
                }
            }
            else if (c == '>' || c == '=' || c == '<' || c == '!') {
                symbol = condition.substr(i, 2);
                if (symbol == c + '=') {
                  curr_condition.push(symbol);
                  ++i;
                }
            }
        }
    }

    if (name)
      curr_condition.push(name);

    if (curr_condition.length) {
      final_condition += translateCondition(lop, curr_condition);
    }

    return final_condition;
  }

  var ComplexCondition = function(data) {
    AbstractCondition.call(this, data);
    this.type = ConditionTokenTypes.ComplexCondition;

    var value = this.value;
    this.value = [];
    for(var i=0; i < value.length; i++) {
      var token = ConditionTokenFactory.createToken(value[i]);
      if (token)
        this.value.push(token);
    }
  };

  belle.extend(ComplexCondition, AbstractCondition);

  ComplexCondition.prototype.setCondition = function(condition) {
    if (!condition || !belle.isInstance(condition, AbstractCondition))
      return;

    this.value = [condition];
  }

  ComplexCondition.prototype.toString = function() {
    var condition = [];
    
    for(var i=0; i < this.value.length; i++) {
      condition.push(this.value[i].toString());
    }

    return condition.join(' ');
  }

  belle.core.Object = Object;
  belle.core.Point = Point;
  belle.core.Asset = Asset;
  belle.core.Timer = Timer;
  belle.core.ConditionLogicalOperatorTypes = ConditionLogicalOperatorTypes;
  belle.core.ConditionLogicalOperator = ConditionLogicalOperator;
  belle.core.ConditionTokenTypes = ConditionTokenTypes;
  belle.core.ConditionOperationTypes = ConditionOperationTypes;
  belle.core.ConditionTokenFactory = ConditionTokenFactory;
  belle.core.ConditionToken = ConditionToken;
  belle.core.SimpleCondition = SimpleCondition;
  belle.core.ComplexCondition = ComplexCondition;
  belle.core.LiteralCondition = LiteralCondition;

}(belle));
