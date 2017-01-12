TEMPLATE = app

# Input.
isEmpty(PREFIX) {
 PREFIX = /usr
}
INCLUDEPATH += actions objects widgets conditions assets
DEPENDPATH += actions objects widgets conditions assets

TARGET = belle
TARGET.path = $$PREFIX/
CONFIG+=debug
QT += core network webkitwidgets

FORMS += mainwindow.ui\
    novel_properties_dialog.ui \
    about.ui \
    add_character.ui \
    save_project_dialog.ui \
    update_elements_dialog.ui

HEADERS      += belle.h\
                scene_manager.h\
                scene.h\
                objects/object.h\
    objects/textbox.h \
    properties_widget.h \
    objects/textbox_editor_widget.h\
    objects/object_editor_widget.h \
    color_pushbutton.h \
    objects/character_properties_widget.h \
    objects/character.h \
    actions/action.h \
    actions_view.h \
    actions_model.h \
    actions/wait.h \
    actions/action_editor_widget.h \
    actions/wait_editor_widget.h \
    actions/dialogue.h \
    actions/dialogue_editor_widget.h \
    objects/dialoguebox.h \
    resource_manager.h \
    actions/slide.h \
    actions/slide_editor_widget.h \
    mimedata.h \
    actions/fade.h \
    actions/fade_editor_widget.h \
    action_catalog_widget.h \
    actions/change_visibility.h \
    actions/change_visibility_editor_widget.h \
    actions/label.h \
    actions/gotolabel.h \
    actions/label_editor_widget.h \
    actions/gotolabel_editor_widget.h \
    objects/button.h \
    combobox.h \
    add_action_dialog.h \
    objects/image.h \
    utils.h \
    resources_view.h \
    actions/branch.h \
    actions/branch_editor_widget.h \
    interaction.h \
    actions/get_user_input.h \
    actions/get_user_input_editor_widget.h \
    actions/changeobjectbackground.h \
    actions/changeobjectbackground_editorwidget.h \
    condition_widget.h \
    condition_dialog.h \
    objects/objectgroup.h \
    objects/objectgroup_editor_widget.h \
    choosefilebutton.h \
    actions/playsound.h \
    actions/playsound_editorwidget.h \
    actions/stopsound.h \
    actions/stopsound_editorwidget.h \
    actions/gotoscene.h \
    actions/gotoscene_editorwidget.h \
    actions/show_menu.h \
    actions/show_menu_editorwidget.h \
    condition_text_edit.h \
    objects/menu.h \
    aboutdialog.h \
    padding.h \
    actions/show.h \
    actions/hide.h \
    actions/show_editor_widget.h \
    actions/hide_editor_widget.h \
    actions/change_background.h \
    actions/change_background_editorwidget.h \
    clipboard.h \
    novel_properties_dialog.h \
    scene_editor_widget.h \
    engine.h \
    drawing_surface_widget.h \
    add_character_dialog.h \
    animatedimage.h \
    choosefontwidget.h \
    simple_http_server.h \
    save_project_dialog.h \
    actions/runscript.h \
    actions/runscript_editorwidget.h \
    update_elements_dialog.h \
    objects/background.h \
    imagefile.h \
    imagetransform.h \
    actions/end.h \
    objectsview.h \
    actions/changestate.h \
    actions/changestate_editorwidget.h \
    assetmanager.h \
    asset.h \
    fontfile.h \
    widgets/object_combobox.h \
    gameobject.h \
    objects/menuoption.h \
    gameobject_editorwidget.h \
    editorwidgetfactory.h \
    gameobjectmanager.h \
    webviewwindow.h \
    sockettimeout.h \
    gameobjectmetatype.h \
    gameobjectfactory.h \
    conditions/conditiontoken.h \
    conditions/abstractcondition.h \
    conditions/simplecondition.h \
    conditions/conditionoperation.h \
    conditions/complexcondition.h \
    conditions/literalcondition.h \
    conditions/conditiontokenfactory.h \
    conditions/conditiontokenmetatype.h \
    conditions/conditionlogicaloperator.h \
    actions/set_game_variable.h \
    actions/set_game_variable_editor_widget.h \
    widgets/variablevalidator.h \
    objects/sound.h \
    multisourceasset.h \
    soundasset.h \
    widgets/filecombobox.h \
    objects/soundeditorwidget.h \
    widgets/gameobjectcombobox.h \
    widgets/soundresourcecombobox.h \
    objects/menueditorwidget.h \
    widgets/slider.h \
    objects/slotbutton.h \
    objects/slotbuttoneditorwidget.h \
    assets/fontasset.h
                

SOURCES      += main.cpp\
                belle.cpp\
                scene_manager.cpp\
                scene.cpp\
    properties_widget.cpp \
    objects/object_editor_widget.cpp \
    color_pushbutton.cpp \
    objects/character_properties_widget.cpp \
    actions/action.cpp \
    actions_view.cpp \
    actions_model.cpp \
    actions/wait.cpp \
    actions/action_editor_widget.cpp \
    actions/wait_editor_widget.cpp \
    actions/dialogue.cpp \
    actions/dialogue_editor_widget.cpp \
    objects/dialoguebox.cpp \
    resource_manager.cpp \
    actions/slide.cpp \
    actions/slide_editor_widget.cpp \
    mimedata.cpp \
    actions/fade.cpp \
    actions/fade_editor_widget.cpp \
    action_catalog_widget.cpp \
    objects/textbox.cpp \
    objects/textbox_editor_widget.cpp \
    actions/change_visibility.cpp \
    actions/change_visibility_editor_widget.cpp \
    actions/label.cpp \
    actions/gotolabel.cpp \
    actions/label_editor_widget.cpp \
    actions/gotolabel_editor_widget.cpp \
    objects/button.cpp \
    combobox.cpp \
    add_action_dialog.cpp \
    objects/image.cpp \
    objects/character.cpp \
    objects/object.cpp \
    resources_view.cpp \
    actions/branch.cpp \
    actions/branch_editor_widget.cpp \
    actions/get_user_input.cpp \
    actions/get_user_input_editor_widget.cpp \
    actions/changeobjectbackground.cpp \
    actions/changeobjectbackground_editorwidget.cpp \
    condition_widget.cpp \
    condition_dialog.cpp \
    objects/objectgroup.cpp \
    objects/objectgroup_editor_widget.cpp \
    choosefilebutton.cpp \
    actions/playsound.cpp \
    actions/playsound_editorwidget.cpp \
    actions/stopsound.cpp \
    actions/stopsound_editorwidget.cpp \
    actions/gotoscene.cpp \
    actions/gotoscene_editorwidget.cpp \
    actions/show_menu.cpp \
    actions/show_menu_editorwidget.cpp \
    condition_text_edit.cpp \
    objects/menu.cpp \
    aboutdialog.cpp \
    padding.cpp \
    actions/show.cpp \
    actions/hide.cpp \
    actions/show_editor_widget.cpp \
    actions/hide_editor_widget.cpp \
    actions/change_background.cpp \
    actions/change_background_editorwidget.cpp \
    clipboard.cpp \
    novel_properties_dialog.cpp \
    scene_editor_widget.cpp \
    engine.cpp \
    drawing_surface_widget.cpp \
    add_character_dialog.cpp \
    animatedimage.cpp \
    choosefontwidget.cpp \
    simple_http_server.cpp \
    save_project_dialog.cpp \
    actions/runscript.cpp \
    actions/runscript_editorwidget.cpp \
    update_elements_dialog.cpp \
    objects/background.cpp \
    imagefile.cpp \
    imagetransform.cpp \
    actions/end.cpp \
    objectsview.cpp \
    actions/changestate.cpp \
    actions/changestate_editorwidget.cpp \
    assetmanager.cpp \
    asset.cpp \
    fontfile.cpp \
    widgets/object_combobox.cpp \
    gameobject.cpp \
    objects/menuoption.cpp \
    gameobject_editorwidget.cpp \
    editorwidgetfactory.cpp \
    gameobjectmanager.cpp \
    webviewwindow.cpp \
    sockettimeout.cpp \
    gameobjectmetatype.cpp \
    gameobjectfactory.cpp \
    conditions/conditiontoken.cpp \
    conditions/abstractcondition.cpp \
    conditions/simplecondition.cpp \
    conditions/conditionoperation.cpp \
    conditions/complexcondition.cpp \
    conditions/literalcondition.cpp \
    conditions/conditiontokenfactory.cpp \
    conditions/conditionlogicaloperator.cpp \
    actions/set_game_variable.cpp \
    actions/set_game_variable_editor_widget.cpp \
    widgets/variablevalidator.cpp \
    objects/sound.cpp \
    multisourceasset.cpp \
    soundasset.cpp \
    utils.cpp \
    widgets/filecombobox.cpp \
    objects/soundeditorwidget.cpp \
    widgets/gameobjectcombobox.cpp \
    widgets/soundresourcecombobox.cpp \
    objects/menueditorwidget.cpp \
    widgets/slider.cpp \
    objects/slotbutton.cpp \
    objects/slotbuttoneditorwidget.cpp \
    assets/fontasset.cpp

RESOURCES += media.qrc
