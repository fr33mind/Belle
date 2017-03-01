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
  var CoreObject = belle.core.Object;

  function AssetManager(data)
  {
    CoreObject.call(this);
    this.loadedAssets = [];
    this.assets = {};
    this.assetsRefCount = {};
    this.load(data);
  }

  belle.extend(AssetManager, CoreObject);

  AssetManager.prototype.load = function (data)
  {
    this.typeToPath = {
      "Image" : "",
      "Audio" : "",
      "Video" : "",
      "Font" : ""
    };
    
    this.data = data;

    if (! data) {
      this.trigger("loaded");
      return;
    }

    var subdirs = data["subdirs"];
    this.typeToPath["Image"] = subdirs["images"];
    this.typeToPath["Audio"] = subdirs["sounds"];
    this.typeToPath["Font"] = subdirs["fonts"];

    var images = data["images"] || [];
    var sounds = data["sounds"] || [];
    var fonts = data["fonts"] || [];

    for(var i=0; i < images.length; i++) {
      this._loadAsset(images[i], "Image");
    }

    for(var i=0; i < sounds.length; i++) {
      this._loadAsset(sounds[i], "Audio");
    }

    for(var i=0; i < fonts.length; i++) {
      this._loadAsset(fonts[i], "Font");
    }
  }

  AssetManager.prototype.loadAsset = function(name, type)
  {
    if (!name)
      return null;

    var data = {
          name: name,
          path: name,
        },
        asset = null;

    if (typeof type == "string")
      data.path = this.getFilePath(name, type);

    if (data.path in this.assets) {
       this.assetsRefCount[data.path] += 1;
       return this.assets[data.path];
    }

    return this._loadAsset(data, type);
  }

  AssetManager.prototype._loadAsset = function(data, type)
  {
    var self = this,
        asset = null,
        name = data.name,
        path = data.path;

    type = typeof type == "string" ?  type.toLowerCase() : "";

    if (!path) {
      path = this.getFilePath(name, type);
    }

    if (type == "image") {
      if ("frames" in data) {
        asset = new belle.graphics.AnimatedImage(path, data.frames, function(){
                                                      self.assetLoaded(this);
                                                  });
      }
      else if (path) {
        asset = new belle.graphics.Image(path, function(){
                                      self.assetLoaded(this);
                                    });
      }
    }
    else if (type == "audio" || type == "sound" || type == "music") {
      var src = data.sources ? this.getFilePaths(data.sources, type) : path;
      asset = new buzz.sound(src, {
                      preload: true
                  });
      asset.name = name;
      asset.bind('canplay error', function() {
        self.assetLoaded(this);
      });
      
      if (! buzz.isSupported())
        this.assetLoaded(asset);
    }
    else if (type == "font") {
      asset = data;
      this._loadFont(asset);
    }

    if (asset) {
      this.assets[path] = asset;
      this.assetsRefCount[path] = 1;
    }

    return asset;
  }

  AssetManager.prototype._loadFont = function(data)
  {
    var _data = {},
        self = this;

    if (data.fontWeight)
      _data.weight = data.fontWeight;
    if (data.fontStyle)
      _data.style = data.fontStyle;

    var font = new FontFaceObserver(data.fontFamily, _data);

    font.load().then(function () {
      self.assetLoaded(data);
    }, function () {
      self.assetLoaded(data);
    });
  }

  AssetManager.prototype.releaseAsset = function(asset)
  {
    if (! asset)
      return;

    var filepath = "";
    for(fpath in this.assets) {
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
    if (this.loadedAssets.indexOf(asset) != -1)
      return;

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
    var path = this.typeToPath[type] ? this.typeToPath[type] : "";
    path = path.length ? path + "/" : path;
    return path + name;
  }

  AssetManager.prototype.getFilePaths = function(paths, type)
  {
    if (!(paths instanceof Array))
      return;

    var full_paths = [],
        path = "";

    for(var i=0; i < paths.length; i++) {
        if (typeof paths[i] == "string")
          path = paths[i];
        else if (paths[i] instanceof Object && typeof paths[i].name == "string")
          path = paths[i].name;

        full_paths.push(this.getFilePath(path, type));
    }

    return full_paths;
  }

belle.AssetManager = AssetManager;

}(belle));
