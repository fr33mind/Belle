/* Copyright (C) 2012-2014 Carlos Pais
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

var objects = belle.objects;
var actions = belle.actions;

var addObject = function (object)
{
    if (game.currentScene)
        game.currentScene.addObject(object);
}

function createAction(data, parent)
{
    var type = data["type"];
    var _Action = belle.actions[type];
    
    if (! _Action) {
        belle.log("'" + type + "' is not a valid action type.");
        return null;
    }
    
    return new _Action(data, parent);
}

function getObject(name, scene) {
    if (scene && name) {
       var objects = scene.objects;
        
       for (var i = 0; i !== objects.length; i++) {
           if (objects[i].name === name)
               return objects[i];
       }
    }
    
    return null;
}

function setGameFile(gamefile)
{
    game.filename = gamefile;
}

function setGameDirectory(dir)
{
    if (dir.length && dir[dir.length-1] !== "/")
        dir += "/";
    game.directory = dir;
}

function load()
{
    if (game && game.data) {
      controller = new belle.GameController(game);
    }
    else
      alert("No game data found!");
}

window.onload = function () {
    if (! belle) {
        alert("Couldn't start the engine. There was a problem reading one of the files.");
        return;
    }
  
    //load();
}

function loadScenes(scenes)
{
    var scene;
    var _scenes = [];
    
    for(var i=0; i < scenes.length; i++) {
        scene = scenes[i];
        
        if (! belle[scene.type]) {
            belle.log("Invalid Scene type: " + scene.type);
            continue;
        }
        
        var _Scene = belle[scene.type];
        var sceneObject = new _Scene(scene);        
        _scenes.push(sceneObject);
    }
    
    return _scenes;
}

function initializeData(data)
{
    var obj;
    var objName;
    
    for (var member in data ) {
        if (member == "resources" || member == "scenes")
            continue;
        
        game[member] = data[member];
    }
    
    game.data = data;
    
    if (game.textSpeed < 0)
        game.textSpeed = 0;
    else if (game.textSpeed > 100)
        game.textSpeed = 100;
    
    game.textDelay = 1000 / game.textSpeed;
    document.title = "Belle - " + game.title;
    
    if ("display" in data && data["display"] == "DOM")
        belle.display.DOM = true;
    
    //init resources
    if (data["resources"]) {
        var resources = data["resources"];
        for (var name in resources) {
            obj = createObject(resources[name]);
 
             if (obj)
                game.resources[name] = obj;
             
        }
    }
    
    //load custom fonts
    /*game.customFonts = [];
    if ("customFonts" in data) {
        var fonts = data["customFonts"];
        for (var i=0; i < fonts.length; i++) {
            //store fonts' names
            if (isFontAvailable(fonts[i]))
                game.customFonts.push(getFontName(fonts[i])); 
        }
    }*/

    //init scenes
    if ("scenes" in data) {
        game.scenes = loadScenes(data["scenes"]);
    }
    
    if ("pauseScreen" in data && "scenes" in data["pauseScreen"]) {
        game.pauseScreen.scenes = loadScenes(data.pauseScreen["scenes"]);
    }

    isGameDataReady();
}

function isGameDataReady() {
    
    var ready = true;
    var scenes = game.scenes;
    var resources = game.resources;
    var actions;
    var objects;
    var objectsLoaded = 0;
    
    /*for(var i=0; i < game.customFonts.length; i++) {
        if (! isFontLoaded(game.customFonts[i])) {
            ready = false;
            break;
        }
    }*/
    
    for(var name in game.resources) {
        if (! resources[name].isReady()) {
            ready = false;
            break;
        }
        objectsLoaded++;
    }
    
    for(var i=0; i < scenes.length; i++) {
        if (! scenes[i].isReady()) {
            ready = false;
            break;
        }
        objectsLoaded++;
        
        actions = scenes[i].actions;
        for(var j=0; j < actions.length; j++){
            if (! actions[j].isReady()) {
                ready = false;
                break;
            }
            if (actions[j].object)
                objectsLoaded++;
        }
        
        objects = scenes[i].objects;
        for(var j=0; j < objects.length; j++){
            if (! objects[j].isReady()) {
                ready = false;
                break;
            }
            objectsLoaded++;
        }
    }

    if (! ready)
        setTimeout(isGameDataReady, 100);
    else {
        display.init();
	if (game.hasNextScene()) 
	  game.nextScene();
        gameLoop();
    }
}

function importgameData(path) {
    var xobj = new XMLHttpRequest();
    if (xobj.overrideMimeType)
        xobj.overrideMimeType("application/json");
    
    xobj.onreadystatechange = function() {
        if(xobj.readyState == 4){
            var gameData = jQuery.parseJSON(xobj.responseText);
            belle.log("Game data loaded!");
            initializeData(gameData);
        }
    };
    
    xobj.open("GET", path, true);
    xobj.send(null);
}

//game's main loop
function gameLoop ()
{   
    if (game.isFinished()) {
      alert("Game finished.\nRefresh page to play again.");
      return;
    }
   
    requestAnimationFrame(gameLoop);
    
    var scene = game.getScene();
    if (scene && scene.isActive()) {
      var action = scene.getAction();
      if (! action || action.isFinished()) {
	  if (scene.hasNextAction()) {
	    scene.nextAction();
          }
	  else {
	    if (game.hasNextScene())
	      game.nextScene();
	    else
              game.setFinished(true);
	  }
      }
    }
    
    display.draw(scene);
}

//Expose public properties
belle.setGameFile = setGameFile;
belle.setGameDirectory = setGameDirectory;
belle.load = load;
belle.addObject = addObject;
belle.createAction = createAction;

}(belle));
