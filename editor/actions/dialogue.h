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

#ifndef DIALOGUE_H
#define DIALOGUE_H

#include "action.h"
#include "dialoguebox.h"
#include "character.h"
#include "sound.h"

class Action;
class DialogueBox;
class Character;
class Sound;

class Dialogue : public Action
{
    Q_OBJECT

    Character* mCharacter;
    QString mCharacterName;
    QString mText;
    bool mAppend;
    Sound* mSound;
    int mSoundVolume;
    bool mTextSpeedEnabled;
    int mTextSpeed;

public:
    explicit Dialogue(QObject *parent = 0);
    Dialogue(const QVariantMap&, QObject *parent = 0);

    QString text();
    void setText(const QString&);

    void setCharacter(Character*);
    void setCharacter(const QString&);
    Character* character();

    void setCharacterName(const QString&);
    QString characterName() const;

    virtual QString displayText() const;
    virtual QVariantMap toJsonObject(bool internal=true) const;

    void updateTextBox();
    void restoreTextBox();
    void activateTextBoxDefaultTextColor();
    void activateCharacterColors();

    bool append() const;
    void setAppend(bool);

    virtual QString editText() const;
    virtual void setEditText(const QString&);

    Character* findCharacter(const QString&);

    Sound* sound() const;
    void setSound(Sound*);
    void setSound(const QString&);

    int soundVolume() const;
    void setSoundVolume(int);

    bool textSpeedEnabled() const;
    void setTextSpeedEnabled(bool);

    int textSpeed() const;
    void setTextSpeed(int);

signals:

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);
    virtual void restoreSceneObject();
    virtual void loadSceneObject();

private slots:
    void onCharacterDestroyed();
    void onCharacterNameChanged(const QString&);
    void onSoundDestroyed(GameObject*);

private:
    void init();
    void removeCharacter();

};

#endif // DIALOGUE_H
