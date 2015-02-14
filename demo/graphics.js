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

(function(belle){

  var GameObject = belle.GameObject,
      CoreObject = belle.core.Object,
      Asset = belle.core.Asset;

  /*** Image ***/

  function Image(path, loadCallback)
  {
    Asset.call(this, path);

    this._image = new window.Image();
    if (loadCallback && typeof loadCallback == "function") {
      var self = this;
      this._image.onload = function() {
        loadCallback.call(self);
      };
    }
    this._image.src = path;
  }

  belle.extend(Image, Asset);

  Image.prototype.isAnimated = function()
  {
    return false;
  }

  Image.prototype.getElement = function()
  {
    return this._image;
  }

  /*** AnimatedImage ***/

  function AnimatedImage(path, frames, loadCallback)
  {
    Asset.call(this, path);

    this.frames = [];
    this._interval = null;
    this.currentFrame = null;
    this.currentFrameNumber = 0;

    for(var i=0; i < frames.length; i++) {
      var img = new window.Image();
      img.src = frames[i]["data"];
      var entry = {
        "img" : img,
        "delay" : frames[i]["delay"]
      };
      this.frames.push(entry);
    }

    if (this.frames && this.frames.length)
      this.currentFrame = this.frames[0];

    if (typeof loadCallback == "function")
      loadCallback();
  }

  belle.extend(AnimatedImage, Image);

  AnimatedImage.prototype.play = function()
  {
    if (! this.frames || this.frames.length < 2 || this._interval !== null)
      return;

    if (! this.currentFrame)
      this.currentFrame = this.frames[0];
    var self = this;
    this._interval = setInterval(function(){ self.nextFrame(); }, this.currentFrame.delay);
  }

  AnimatedImage.prototype.stop = function()
  {
    if (this._interval !== null)
      clearInterval(this._interval);
    this._interval = null;
  }

  AnimatedImage.prototype.nextFrame = function()
  {
    this.currentFrameNumber++;
    if (this.currentFrameNumber >= this.frames.length)
      this.currentFrameNumber = 0;
    this.currentFrame = this.frames[this.currentFrameNumber];
    this.trigger("frameChanged");
  }

  AnimatedImage.prototype.isAnimated = function()
  {
    return (this.frames.length > 1);
  }

  AnimatedImage.prototype.getElement = function()
  {
    return this.currentFrame["img"];
  }

  /*** Color ***/

  function Color(components)
  {
    var error = false;
    if (! components || components.length < 4) {
      error = true;
      belle.log("Color(components) : Missing one or more color component(s)");
    }

    if (error) {
      this.red = 0;
      this.green = 0;
      this.blue = 0;
      this.alpha = 0;
    }
    else {
      this.red = components[0];
      this.green = components[1];
      this.blue = components[2];
      this.alpha = components[3];
    }
  }

  Color.prototype.toString = function()
  {
    return 'rgba(' + this.red + ',' + this.green + ',' + this.blue + ',' + this.alphaF() + ')';
  }

  Color.prototype.componentToHex = function (c)
  {
    var hex = c.toString(16);
    return hex.length == 1 ? "0" + hex : hex;
  }

  Color.prototype.toHex = function()
  {
    return "#" + this.componentToHex(this.red) + this.componentToHex(this.green) + this.componentToHex(this.blue);
  }

  Color.prototype.alphaF = function()
  {
    return this.alpha / 255;
  }

  Color.prototype.serialize = function()
  {
    return [this.red, this.green, this.blue, this.alpha];
  }

  function Background(image, color)
  {
    this.image = image ? image : null;
    this.color = color ? color : null;
    this.opacity = color ? color.alpha : 0;
  }

  /*** Background - Used for background in Scene and other objects ***/

  Background.prototype.setColor = function(color)
  {
    if (color instanceof Array)
      color = new Color(color);

    this.color = color;
    this.setOpacity(color ? color.alpha : 0);
  }

  Background.prototype.setImage = function(image)
  {
    this.image = image;
  }

  Background.prototype.setOpacity = function(opacity)
  {
    this.opacity = opacity;
  }

  Background.prototype.isValid = function()
  {
    if (! this.opacity)
      return false;
    if (! this.image && ! this.color)
      return false;
    return true;
  }

  belle.graphics.AnimatedImage = AnimatedImage;
  belle.graphics.Image = Image;
  belle.graphics.Color = Color;
  belle.graphics.Background = Background;

}(belle));
