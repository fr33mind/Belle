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

  var ImageFormats = ["png", "jpg", "jpeg", "gif", "tiff", "bmp", "svg"];
  var AudioFormats = ["mp3", "aac", "ogg", "oga", "webm", "wav", "flac", "m4a"];
  var VideoFormats = ["mp4", "m4v", "webm", "ogv"];
  var FontFormats = ["ttf", "otf", "eot", "woff"];
  var Object = belle.core.Objet,
      Image = belle.graphics.Image;

  function AssetManager(data)
  {
    belle.core.Object.call(this);
    this.loadedAssets = [];
    this.assets = {};
    this.assetsRefCount = {};
    this.load(data);
  }

  belle.extend(AssetManager, belle.core.Object);

  AssetManager.prototype.load = function (data)
  {
    this.subdirs = {
      "image" : "",
      "audio" : "",
      "video" : "",
      "font" : ""
    };

    if (! data) {
      this.trigger("loaded");
      return;
    }

    this.subdirs = data["subdirs"];
    var images = data["image"];
    var audio = data["audio"];

    for(var i=0; i < images.length; i++) {
      this.loadAsset(images[i], "Image");
    }

    for(var i=0; i < audio.length; i++) {
      this.loadAsset(audio[i], "Audio");
    }
  }

  AssetManager.prototype.loadAsset = function(data, type)
  {
    var data = typeof data == "string" ? {name: data} : data,
        name = data.name,
        filepath = "",
        asset = null;

    filepath = this.getFilePath(name, type);

    if (filepath in this.assets) {
       this.assetsRefCount[filepath] += 1;
       return this.assets[filepath];
    }

    data.filepath = filepath;
    asset = this._loadAsset(data, type);
    this.assets[filepath] = asset;
    this.assetsRefCount[filepath] = 1;
    return asset;
  }

  AssetManager.prototype._loadAsset = function(data, type)
  {
    var self = this;
    if (! type)
      type = "";
    type = type.toLowerCase();

    if (type == "image") {
      if ("frames" in data) {
        return new belle.graphics.AnimatedImage(data.frames, function(){
                                                      self.assetLoaded(this);
                                                  });
      }
      else if ("name" in data) {
        return new belle.graphics.Image(data.filepath, function(){
                                      self.assetLoaded(this);
                                    });
      }
    }
    else if (type == "audio" || type == "sound" || type == "music") {
      var sound = new buzz.sound(data.filepath);
      sound.bind('canplay', function() {
        self.assetLoaded(this);
      });
      if (! buzz.isSupported())
        this.assetLoaded(sound);
      return sound;
    }

    return null;
  }

  AssetManager.prototype.releaseAsset = function(asset)
  {
    if (! asset)
      return;

    var filepath = "";
    for(fpath in assets) {
      if (this.assets[fpath] == asset) {
        filepath = fpath;
        break;
      }
    }

    if (filepath.length) {
      this.assetsRefCount[filepath] -= 1;
      if (this.assetsRefCount[filepath] <= 0)
        this._deleteAsset(filepath);
    }
  }

  AssetManager.prototype._deleteAsset = function(path)
  {
    var asset = this.assets[path];
    var index = this.loadedAssets.indexOf(asset);
    if (index != -1)
      this.loadedAssets.splice(index, 1);
    delete this.assets[path];
    delete this.assetsRefCount[path];
  }

  AssetManager.prototype.getAsset = function(name, type)
  {
    var filepath = this.getFilePath(name, type);
    if (filepath in assets)
      return this.assets[filepath];
    return null;
  }

  AssetManager.prototype.getAssets = function(loaded)
  {
    var assets = [];
    if (loaded)
      assets = this.loadedAssets;
    else {
      for(var path in this.assets)
        assets.push(this.assets[path]);
    }
    return assets;
  }

  AssetManager.prototype.isLoaded = function ()
  {
    return (this.size(true) == this.size());
  }

  AssetManager.prototype.assetLoaded = function(asset)
  {
    this.loadedAssets.push(asset);
    this.trigger("assetLoaded", {
      asset: asset,
      loaded: this.loadedAssets.length,
      total: this.size()
    });

    if (this.isLoaded())
      this.trigger("loaded");
  }

  AssetManager.prototype.size = function(loaded)
  {
    return this.getAssets(loaded).length;
  }

  AssetManager.prototype.guessType = function(name)
  {
    if (name.indexOf(".") !== -1) {
      var parts = name.trim().split(".");
      var ext = parts[parts.length-1];

      if (ImageFormats.indexOf(ext) !== -1)
        return "Image";
      if (AudioFormats.indexOf(ext) !== -1)
        return "Audio";
      if (VideoFormats.indexOf(ext) !== -1)
        return "Video";
      if (FontFormats.indexOf(ext) !== -1)
        return "Font";
    }

    return "Unknown";
  }

  AssetManager.prototype.getFilePath = function(name, type)
  {
    if (! type)
      type = guessType(name);
    type = type.toLowerCase();
    var path = this.subdirs[type] ? this.subdirs[type] : "";
    path = path.length ? path + "/" : path;
    return path + name;
  }

belle.AssetManager = AssetManager;

}(belle));
