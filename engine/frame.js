(function(belle){

  var Background = belle.graphics.Background,
      GameObject = belle.GameObject,
      Image = belle.graphics.Image;

  /*** Frame - Base class for all visible objects ***/

  function Frame(data, parent)
  {
    GameObject.call(this, data, parent);
    this.background = new Background();
    this.redraw = true;
    this.paintX = false;
    this.paintY = false;
    this.paintWidth = false;
    this.paintHeight = false;
    this.visible = true;
    this.x = 0;
    this.y = 0;
    this.width = 0;
    this.height = 0;

    Frame.prototype.load.call(this, data);
  }

  belle.extend(Frame, GameObject);

  Frame.prototype.load = function(data)
  {
    if (! data)
      return false;

    var backgroundImage = null,
        backgroundColor = null,
        assetManager = this.getGame().getAssetManager();

    if ("x" in data)
      this.setX(data.x);

    if ("y" in data)
      this.setY(data.y);

    if ("width" in data)
      this.setWidth(data.width);

    if ("height" in data)
      this.setHeight(data.height);

    if ("visible" in data)
      this.setVisible(data.visible);

    if ("backgroundImage" in data)
      backgroundImage = assetManager.loadAsset(data.backgroundImage, "Image");
    if ("backgroundColor" in data)
      backgroundColor = data.backgroundColor;

    this.setBackgroundImage(backgroundImage);
    this.setBackgroundColor(backgroundColor);

    return true;
  }

  Frame.prototype.mapXFromName = function(name)
  {
    var scene = this.getScene();
    if (! scene)
      return null;

    if (name.indexOf("left") != -1) {
      if (name.indexOf("out") != -1)
        return -1 * this.width;
      else
        return 0;
    }

    if (name.indexOf("center") != -1)
      return (scene.width / 2) - (this.width / 2);

    if (name.indexOf("right") != -1) {
      if (name.indexOf("out") != -1)
        return scene.width;
      else
        return scene.width - this.width;
    }

    return null;
  }

  Frame.prototype.mapYFromName = function(name)
  {
    var scene = this.getScene();
    if (! scene)
      return null;

    if (name.indexOf("top") != -1) {
      if (name.indexOf("out") != -1)
        return this.height * -1;
      else
        return 0;
    }

    if (name.indexOf("center") != -1)
      return (scene.height / 2) - (this.height / 2);

    if (name.indexOf("bottom") != -1) {
      if (name.indexOf("out") != -1)
        return scene.height;
      else
        return scene.height - this.height;
    }

    return null;
  }

  Frame.prototype.setX = function(x)
  {
    if (typeof x == "string" && ! belle.isNumber(x)) {
     x = this.mapXFromName(x);
    }

    if (belle.isNumber(x)) {
      x = parseInt(x);
      //FIXME: Use relative positions, not global
      if (this.parent && belle.isNumber(this.parent.x))
        x -= this.parent.x;
      this.x = x;
      this.update();
    }
  }

  Frame.prototype.setY = function(y)
  {
    if (typeof y == "string" && ! belle.isNumber(y)) {
      y = this.mapYFromName(y);
    }

    if (belle.isNumber(y)) {
      y = parseInt(y);
      //FIXME: Use relative positions, not global
      if (this.parent && belle.isNumber(this.parent.y))
        y -= this.parent.y;
      this.y = y;
      this.update();
    }
  }

  Frame.prototype.moveTo = function(x, y)
  {
      this.setX(x);
      this.setY(y);
  }

  Frame.prototype.globalX = function()
  {
      if (this.parent && belle.isNumber(this.parent.x))
          return this.x + this.parent.x;
      return this.x;
  }

  Frame.prototype.globalY = function()
  {
      if (this.parent && belle.isNumber(this.parent.y))
          return this.y + this.parent.y;
      return this.y
  }

  Frame.prototype.setWidth = function(width)
  {
    if (this.parent && belle.isInstance(this.parent, Frame) && belle.utils.isPercentSize(width))
        width =  parseInt(width) * this.parent.width / 100;

    this.width = width;
  }

  Frame.prototype.setHeight = function(height)
  {
    if (this.parent && belle.isInstance(this.parent, Frame) && belle.utils.isPercentSize(height))
        height =  parseInt(height) * this.parent.height / 100;

    this.height = height;
  }

  Frame.prototype.setPosition = function(x, y)
  {
    this.setX(x);
    this.setY(y);
  }

  Frame.prototype.show = function()
  {
    this.setVisible(true);
  }

  Frame.prototype.hide = function()
  {
    this.setVisible(false);
  }

  Frame.prototype.setVisible = function(visible)
  {
    if (this.visible == visible)
      return;

    this.visible = visible;
    if (this.background.image && this.background.image.isAnimated()) {
      if (this.visible)
        this.background.image.play();
      else
        this.background.image.stop();
    }
    this.update();
  }

  Frame.prototype.isVisible = function()
  {
    return this.visible;
  }

  Frame.prototype.contains = function(px, py)
  {
    if (! this.visible)
      return false;

    var x = this.globalX(),
        y = this.globalY();
    if ( px >= x && px <=  x+this.width && py >= y && py <= y+this.height)
        return true;
    return false;
  }

  Frame.prototype._bindImage = function(image)
  {
    if (image && image.isAnimated()) {
      image.bind("frameChanged", this, function() {
        this.update();
      }, true);

      if (this.visible)
        image.play();
    }
  }

  Frame.prototype._unbindImage = function(image)
  {
    if (image && image.isAnimated()) {
      image.stop();
      image.unbind("frameChanged", this);
    }
  }

  Frame.prototype.setBackgroundImage = function(image)
  {
    if (this.background.image == image)
      return;

    var oldImage = this.background.image;

    if (belle.isInstance(image, Image)) {
        this.background.setImage(image);
        this._bindImage(this.background.image);
    }
    else if (typeof image == "string" || typeof background == "object") {
        var assetManager = this.getGame().getAssetManager();
        if (assetManager)
          this.background.setImage(assetManager.loadAsset(image, "Image"));;
    }

    this._unbindImage(oldImage);

    this.update();
  }

  Frame.prototype.setBackgroundColor = function(color)
  {
    if (this.background.color == color)
      return;

    this.background.setColor(color);
    this.update();
  }

  Frame.prototype.setBackgroundOpacity = function(opacity)
  {
    this.background.setOpacity(opacity);
    this.update();
  }

  Frame.prototype.paint = function(ctx)
  {
    if (! this.visible)
      return;

    var x = this.globalX(),
        y = this.globalY();

    if (this.background.image) {
        ctx.drawImage(this.background.image.getElement(), x, y, this.width, this.height);
    }
    else if (this.background.color) {
        ctx.fillStyle = this.background.color.toString();
        ctx.fillRect(x, y, this.width, this.height);
    }

    this.paintX = x;
    this.paintY = y;
    this.paintWidth = this.width;
    this.paintHeight = this.height;
  }

  Frame.prototype.clear = function (context)
  {
    //paint* variables already contain the proper position and size values
    if (this.paintX !== false && this.paintY !== false) {
      context.clearRect(this.paintX, this.paintY, this.paintWidth, this.paintHeight);
      this.paintX = this.paintY = this.paintWidth = this.paintHeight = false;
    }
  }

  Frame.prototype.update = function()
  {
    if (this.parent && typeof this.parent.update == "function")
      this.parent.update();
    this.redraw = true;
  }

  Frame.prototype.isPainted = function()
  {
    return (this.paintX !== false || this.paintY !== false);
  }

  Frame.prototype.serialize = function()
  {
    var data = GameObject.prototype.serialize.call(this);

    data["x"] = this.globalX();
    data["y"] = this.globalY();
    if (this.background.image)
      data["backgroundImage"] = this.background.image.getPath();
    if (this.background.color)
      data["backgroundColor"] = belle.serialize(this.background.color);
    data["width"] = this.width;
    data["height"] = this.height;
    return data;
  }

  belle.Frame = Frame;

}(belle));
