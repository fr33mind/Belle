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

#include "action_editor_widget.h"
#include "dialogue.h"
#include "scene_manager.h"
#include "textbox.h"
#include "character.h"

class Dialogue;
class TextBox;

class DialogueEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QComboBox* mChooseCharacterWidget;
    QComboBox* mChooseTextBoxWidget;
    QCheckBox* mAppendCheckbox;
    QTextEdit* mTextEdit;
    QList<Object*> mOutputBoxes;
    QList<Character*> mCharacters;
    QCheckBox* mWaitCheckBox;

public:
    explicit DialogueEditorWidget(QWidget *parent = 0);
    virtual void updateData(Action*);
    virtual bool eventFilter(QObject *, QEvent *);

signals:

public slots:

private slots:
    void onTextEdited();
    void onTextBoxChanged(int);
    void onCharacterChanged(int);
    void onTextBoxHighlighted(int);
    void onCharacterHighlighted(int);
    void onCharacterNameChanged(const QString&);
    void onWaitOnFinishedChanged(bool);
    void appendToggled(bool);

private:
    bool isValidOutputBox(Object*);
    void  setTextInOutputBox();

};

#endif // DIALOGUE_EDITOR_WIDGET_H
