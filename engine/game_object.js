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

  var CoreObject = belle.core.Object;

  /*** GameObject - Base class for all game objects ***/
  
  function GameObject(data, parent)
  {
    CoreObject.call(this, parent);
    data = data || {};
    this.name = data.name;
    this.type = data.type;
    this.data = data;
    this._scene = this.getScene();
    this._game = this.getGame();
    this._gameModel = this.getGameModel();
  }

  belle.extend(GameObject, CoreObject);

  GameObject.prototype.getScene = function()
  {
    if (this._scene)
      return this._scene;
    if (this.parent && this.parent instanceof belle.Scene)
      return this.parent;
    if (this.parent && typeof this.parent.getScene == "function")
      return this.parent.getScene();
    if (this instanceof belle.Scene)
      return this;
    return null;
  }

  GameObject.prototype.getGame = function()
  {
    if (this._game)
      return this._game;

    var gameModel = this.getGameModel();
    if (gameModel)
      return gameModel.getGame();

    if (this.parent && typeof this.parent.getGame == "function")
      return this.parent.getGame();

    if (this.parent instanceof belle.Game)
      return this.parent;

    return null;
  }

  GameObject.prototype.getGameModel = function()
  {
    if (this._gameModel)
      return this._gameModel;

    if (this.parent && typeof this.parent.getGameModel == "function")
      return this.parent.getGameModel();
    if (this.parent && this.parent instanceof belle.GameModel)
      return this.parent;
    return null;
  }

  GameObject.prototype.reload = function()
  {
    this.load(this.data);
  }

  GameObject.prototype.load = function(data)
  {
  }

  GameObject.prototype.serialize = function()
  {
    var data = {}
    data["name"] = this.name;
    data["type"] = this.type;
    return data;
  }

  belle.GameObject = GameObject;

}(belle));
