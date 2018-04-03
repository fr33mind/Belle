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

#ifndef DIALOGUE_EDITOR_WIDGET_H
#define DIALOGUE_EDITOR_WIDGET_H

#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QSlider>

#include "action_editor_widget.h"
#include "dialogue.h"
#include "scene_manager.h"
#include "textbox.h"
#include "character.h"
#include "object_combobox.h"
#include "soundresourcecombobox.h"

class Dialogue;
class TextBox;
class SoundResourceComboBox;

class DialogueEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    ObjectComboBox* mChooseCharacterWidget;
    ObjectComboBox* mChooseTextBoxWidget;
    QCheckBox* mAppendCheckbox;
    QTextEdit* mTextEdit;
    QCheckBox* mWaitCheckBox;
    QCheckBox* mPlaySoundCheckBox;
    SoundResourceComboBox* mSoundComboBox;
    QSlider* mSoundVolumeSlider;

public:
    explicit DialogueEditorWidget(QWidget *parent = 0);
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void setGameObject(GameObject*);

signals:

protected:
    virtual void updateData(GameObject*);

private slots:
    void onTextEdited();
    void onTextBoxChanged(Object*);
    void onCharacterChanged(Object*);
    void onCharacterChanged(const QString&);
    void onTextBoxHighlighted(Object*);
    void onCharacterHighlighted(Object*);
    void onWaitOnFinishedChanged(bool);
    void appendToggled(bool);
    void onPlaySoundCheckBoxToggled(bool);
    void onSoundChanged(GameObject*);
    void onSoundVolumeChanged(int);

private:
    void  setTextInOutputBox();
    void setSoundEnabled(bool);
    void initSoundWidgets(Dialogue*);

};

#endif // DIALOGUE_EDITOR_WIDGET_H
