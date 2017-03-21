## Belle 0.7b | 21/03/2017

### [General]
- Added SlotButton resource.
- Added Font resource.
- Implemented effectively padding for objects (currently only usable in TextBox).
- Fixed and improved broken behaviour of ChangeColor (now called ChangeObjectBackground).
- Added support for images in ChangeObjectBackground.
- Added Sound resource.
- Renamed ChangeGameVariable to SetGameVariable.

#### [Editor]
- Improved loading process of engine's path at startup.
- Fixed crash when removing image from ChangeBackground action.
- Fixed possible crash when removing ShowMenu action.
- Added synchronization for events' actions.
- Improved ObjectGroup management.
- Added option to align Menu in ShowMenu action.
- Fixed application closing when user clicked cancel on Save Project dialog.
- Added dialog for managing event actions.
- Added support for moving event actions.
- Fixed no height limit in actions' items.
- Added 4 different colors for each group of actions.
- Removed border and corner radius from actions' items.
- Fixed possible crash after closing the main window.
- Limited display text to 3 lines for any action.
- Added font style and font weight properties to TextBox.
- Added some shortcuts to easily and quickly write dialogues.
- Fixed "Paste" option not showing when clicking on an action.
- Fixed selection when pasting and moving actions.
- Fixed action items not being dropped in correct order.
- Corrected copy/cut behaviour for objects and actions.
- Fixed selection border width and color.
- Speeded up the opening of projects.
- Added a tooltip to sliders to show their value.
- Fixed display text in ChangeBackground.
- Fixed GoToScene not setting scene object when loaded from file.
- Fixed some issues when copying or cutting actions.
- Fixed "New Project" option not initializing project properly.
- Added Menu resource.
- Updated ShowMenu to work with Menu resources.
- Fixed image assets not being released in some cases when used in event actions.
- Added a preview of the changes when selecting a ChangeObjectBackground action.
- Fixed crash when removing current image in ChangeBackground.
- Renamed "Novel" menu to "Project".
- Set objects' background opaque but without color by default.
- Fixed sync property not loading correctly.
- Added delete shortcut in resources view.
- Changed resources to open up for editing right after creating them.
- Fixed and improved condition dialog.
- Fixed some issues with aspect ratio option.
- Improved GoToScene action management.
- Set resources view to show resource type icon.
- Limited possible characters to use when specifying a variable name.
- Fixed issue when removing multiple actions.
- Set Ctrl+R shortcut to run the project.
- Added a checkup of engine directory at startup.
- Fixed badly generated names for assets.
- Fixed assets not being removed from disk when removed from project.
- Improved management of GoToLabel.
- Fixed view not scrolling to action after creating it.
- Automatically select an action after creating it.
- Changed ShowMenu's display text.
- Changed Branch's display text.
- Fixed and improved actions' drag and drop.
- Set ellipsis by line in actions' text.
- Added proper keyboard navigation in actions' view.
- Allowed enabling/disabling visibility transitions.
- Set first scene selected after project import.
- Created tab widget with main and pause scenes.
- Improved HTTP server.
- Improved built-in browser.
- Fixed actions' view scrolling to top after removing an action.
- Set red background in name's input if name is not valid.
- Set run directory to be removed on close.
- Upgraded Qt framework from Qt4 to Qt5.
- Other minor fixes.

#### [Engine]
- Improved management of game state in mouse move event.
- Fixed fade out not working when stopping a sound.
- Fixed loop issue when playing a sound.
- Fixed ChangeBackground not removing background image.
- Fixed TextBox not updating with empty text.
- Improved loading accuracy of fonts.
- Corrected vertical alignment of text in TextBox.
- Added confirmation when overwriting a saved game.
- Added confirmation when loading a saved game.
- Fixed engine not loading when using more than one gif.
- Fixed slide not working when duration was zero.
- Run show/hide transitions regardless if object is visible/hidden.
- Fixed sounds not playing after loading a saved game.
- Removed DOM support.
- Other minor fixes.

## Belle 0.6.1a | 01/07/2015

#### [Engine]
- Minor improvement when executing simultaneous actions.
- Fixed Menu not disappearing when stopping Show Menu action.
- Fixed variables not being updated in Dialogue action.
- Fixed bug when executing a Branch action.
- Fixed small issue after removing an object from scene.

## Belle 0.6a | 14/06/2015

### [General]
- Added append option to Dialogue actions.
- Improved Menu object.

#### [Editor]
- Set fade type correctly.
- Fixed action not loading target object in some cases.
- Improved target object selection in action's editor.
- Improved Resource and clones synchronization.
- Minor improvements on simple http server.
- Fixed object combobox in ChangeColor editor.
- Fixed crash when adding new option to menu.
- Other minor fixes.

#### [Engine]
- Fixed empty lines not being rendered in TextBox.
- Init game variable properly.
- Fixed initialization of Character states.
- Stop any mouse move actions when a mouse leave event occurs.
- Fixed audio preloading.
- Fixed and improved some actions.
- Added ActionGroup.
- Fixed some hovering issues.
- Improved event handling in objects.
- Improved management of executed actions in GameModel.
- Overall improvements.

## Belle 0.5a | 13/02/2015

#### [Editor]
- Added "New Project" option.
- Added synchronization between resources and cloned objects on scene(s). (not active by default)
- Assets are now exported to a separate file (assets.js).
- Improved usability of condition's text edit.
- Implemented character state in Show action.
- Fixed ChangeState action not loading correctly.
- Set proper detection of animated images.
- Fixed images not showing in add character dialog.
- Fixed issues in Show Action.
- Improved editing of Slide action.
- Fixed object z-order when importing project.
- Fixed opacity in ObjectGroup objects.
- Fixed crash when switching character states.
- Fixed crash when switching to a ChangeBackground action.
- Improved ObjectGroup editing.
- Fixed many (other) bugs.

#### [Engine]
- Major redesign and rewrite.
- Improved pause screen.
- Improved load screen.
- Added support for animated images in canvas.
- Fixed strange behaviour with text baseline due to floating point coordinates.
- Removed stroke when drawing the object's background.
- Fixed drawing bug when hovering a button.
- Added meta tag to force IE to use the correct document mode.
- Improved overall event detection.
- Improved Slide animation.
- Fixed many (other) bugs.

## Belle 0.4.2a | 12/09/2014

#### [General]
- Added ChangeState action.
- Fixed a couple of issues with the Character object.

#### [Editor]
- Improved add character dialog window.
- Added a button to edit character states after being created.
- Added white border to invisible objects.
- Added objects view widget.
- TextBox, DialogueBox and Button objects now stay over other objects, by default.
- Fixed crash when changing states in character.
- Fixed background color in objects when not using any color.
- Choose file buttons now remember last used directory.
- Fixed crash when trying to change a deleted object/action.
- Fixed some issues when changing TextBox objects in Dialogue's action editor.
- Set image transforms smoother.

## Belle 0.4.1a | 19/08/2014

- Added dialog to save changes when closing the editor.
- Fixed EndNovel action not being created in the editor.
- Fixed temporary background disappearing after save.
- Fixed minor issue that was preventing Menu from working properly.
- Fixed action not loading object when inside another action.
- Fixed bug in Show/Hide actions that prevented selecting an object.
- Added align options to objects.
- Improved editing of menus and dialogue boxes.
- Added some other minor improvements.
- Fix save/load slots not appearing when in DOM mode.

## Belle 0.4a | 15/06/2014

#### [Editor]
- Added integrated browser window (not active by default).
- Added support for animated images as background.
- Added (new) opacity property to objects.
- Improved rounded corners editing options.
- Fixed rounded corners when using an image as background.
- Added option to maintain images' aspect ratio.
- Added Save Project option.
- Added pause screen scenes' manager.
- Improved presentation of some actions in the actions' window.
- Improved StopSound properties editor.
- Added (new) RunScript action.
- Fixed resources not being updated when being edited some times.
- Added possibility to edit actions already assigned to an object's event.
- Set initial name for Images equal to their file name.
- Fixed some issues with placeholder text in TextBoxes.
- Fixed shortcuts not working in scene view.
- Fixed some crashes and other bugs.

#### [Engine]
- Added pause screen.
- Fixed offline distribution.
- Added RunScript action.
- Added save/load mechanism using browser's local storage.
- Improved DOM and canvas rendering.
- Improved sound management.
- Fixed issue when skipping Slide action.
- Improved overall code structure.
- Improved display in smaller screens.
- Renamed EndNovel action to End.
- Fixed many bugs.

## Belle 0.3a | 05/03/2013

#### [Editor]
- Added support for animated images (e.g. gifs).
- Improved memory management.
- Added option to select different browser in settings.
- Fixed image duplication when running a project multiple times.
- Improved font management.
- Added support for custom fonts.
- Added font option to textbox object.
- Fixed allow skipping option in wait action.
- Set wait action maximum duration to 3600 seconds.
- Improved resource editing and management.
- Improved scene view.
- Improved text display in dialogue actions.
- Fixed image object not appearing in scene view when copied from an image resource.
- Fixed scene double delete.
- Fixed bug in show action not showing available objects.
- Changed the behaviour on how objects are added to the scene.
- Set image scaling to use bilinear filtering for a smoother scaled image. 
- Fixed bug in view menu options after restarting application.
- Fixed some missing data when importing a project.
- Fixed many crashes and other bugs.

#### [Engine]
- Added DOM mode.
- Added support for animated images (e.g. gifs).
- Added support for older browsers (e.g. Internet Explorer 7 and 8).
- Improved code organization.
- Improved integration with websites.
- Added loading bar.
- Improved canvas painting.
- Improved event detection.
- Improved text display when using canvas.
- Fixed many bugs.

## Belle 0.2a | 17/12/2012

#### [Editor]
- Improved text decoding.
- Added capability for loading and saving window geometry and state.
- Fixed duplicate object in dialogue's editor.
- Fixed bug when importing name from game file.
- Fixed character's name bug.
- Renamed SetGameVariable to ChangeGameVariable.
- Fixed exporting character's name in dialog action.
- Added option to control text speed.
- Minor fix in condition dialog.
- Renamed AddObjectDialog class to AddCharacterDialog.
- Fixed export game file.
- Improved project export.

#### [Engine]
- Renamed canvas's ids.
- Fixed bug(s) in change background action.
- Fixed crash when an image is missing.
- Fixed character's name not showing in dialogue.
- Improved text speed option.
- Improved scalability.
- Fixed textbox's text alignment when scaling.
- Fixed FPS label.
- Renamed SetGameVariable to ChangeGameVariable.
- Other minor fixes.

## Belle 0.1.1a | 20/11/2012

#### [Editor]
- Fixed color support for scene's background.
- Added color support in ChangeBackground action.
- Added "backgroundColor" property for scene when exporting game file.
- Replaced "background" property with "backgroundImage" when exporting game file.
- Other minor tweaks.

#### [Engine]
- Fixed display in Firefox.
- Added support to use colors as scene's background.
- Added color support in ChangeBackground action.
- Updated to read new properties from game file.
- Other minor tweaks.

## Belle 0.1a | 15/11/2012

- First release.
