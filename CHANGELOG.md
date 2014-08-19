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