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

  var Game = belle.Game;
  var GameView = belle.views.GameView;
  var LoadView = belle.views.LoadView;

  function GameController(data)
  {
    if (! buzz.isSupported())
      alert("Audio is not supported by your browser");

    this.views = {};
    this.game = new Game(data);
    this.game.controller = this;
    this.view = null;
    this.touchEnabled = false;
    if (! $('body').find("#belle").length) {
      $('body').append('<div id="belle"></div>');
    }
    this.container = $('body').find("#belle");
    this.setupGameEvents();
    this.setupUserEvents();

    this.views["main"] = new GameView(this.container, this);
    this.views["pause"] = new GameView(this.container, this);
    this.views["load"] = new LoadView(this.container, this);

    this.views["main"].setModel(this.game.getMainModel());
    this.views["pause"].setModel(this.game.getPauseModel());

    for(var view in this.views) {
      this.views[view].setWidth(this.game.getProperty("width"));
      this.views[view].setHeight(this.game.getProperty("height"));
    }

    if (this.game.isReady())
      this.game.resume();
    else
      this.setView("load");
  }

  GameController.prototype.setupGameEvents = function()
  {
    this.game.getAssetManager().bind("assetLoaded", this, function(data) {
      this.views["load"].update(data);
    });

    this.game.bind("paused", this, function() {
      this.setView("pause");
    });

    this.game.bind("resumed", this, function() {
      this.setView("main");
    });

    this.game.bind("finished", this, function() {
      setTimeout(function() {
        alert("Game finished.\nRefresh page to play again.");
      }, 500);
    });
  }

  GameController.prototype.setupUserEvents = function()
  {
    var controller = this;

    //equivalent to mousedown
    $(document).bind('touchstart', function(e){
      var touch = e.originalEvent.touches[0] || e.originalEvent.changedTouches[0];
      if (touch) {
        controller.touchEnabled = true;
        e.preventDefault();
        e.stopPropagation();
        e.pageX = touch.pageX;
        e.pageY = touch.pageY;
        if (controller.view)
          controller.view.mouseDown(e);
      }
    });

    //equivalent to mouseup
    $(document).bind('touchend', function(e){
      var touch = e.originalEvent.touches[0] || e.originalEvent.changedTouches[0];
      if (touch) {
        controller.touchEnabled = true;
        e.preventDefault();
        e.stopPropagation();
        e.pageX = touch.pageX;
        e.pageY = touch.pageY;
        if (controller.view)
          controller.view.mouseUp(e);
      }
    });

    //equivalent to mousemove
    $(document).bind('touchmove', function(e){
      var touch = e.originalEvent.touches[0] || e.originalEvent.changedTouches[0];
      if (touch) {
        controller.touchEnabled = true;
        e.preventDefault();
        e.stopPropagation();
        e.pageX = touch.pageX;
        e.pageY = touch.pageY;
        if (controller.view)
          controller.view.mouseMove(e);
      }
    });

    $(document).bind('mousedown', function(e){
      if (controller.touchEnabled)
        return;
      if (controller.view)
        controller.view.mouseDown(e);
    });

    $(document).bind('mouseup', function(e){
      if (controller.touchEnabled)
        return;
      if (controller.view)
        controller.view.mouseUp(e);
    });

    $(document).bind('mousemove', function(e){
      if (controller.touchEnabled)
        return;
      if (controller.view)
        controller.view.mouseMove(e);
    });

    $(document).keyup(function(ev) {
      if (ev.keyCode == 27)
        return controller.pauseTriggered();

      if (controller.view)
        controller.view.keyUp(ev);
    });

    $(window).resize(function(ev){
      controller.view.resize(ev);
    });

    $(window).bind('load', function(e){
      if (controller.view) {
        //force complete redraw
        controller.view.clear();
      }
    });

  }

  GameController.prototype.pauseTriggered = function()
  {
    this.game.setPaused(! this.game.isPaused());
  }

  GameController.prototype.setView = function(view)
  {
    if (! view)
      return;

    if (typeof view == "string")
      view = this.views[view] || null;

    if (this.view)
      this.view.hide();

    this.view = view;
    this.view.show();
  }

  belle.GameController = GameController;

}(belle));
