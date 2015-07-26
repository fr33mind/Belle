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

#ifndef PLAYSOUND_H
#define PLAYSOUND_H

#include "action.h"

class PlaySound : public Action
{
    Q_OBJECT

    int mVolume;
    Asset* mSound;
    bool mLoop;

public:
    static ActionInfo Info;

public:
    explicit PlaySound(QObject *parent = 0);
    PlaySound(const QVariantMap&, QObject *parent = 0);

    void setSoundPath(const QString&);
    QString soundPath();
    QString soundName();

    void setVolume(int);
    int volume();

    void setLoop(bool);
    bool loop();

    QVariantMap toJsonObject();
    virtual QString displayText() const;

private:
   void init();

};

#endif // PLAYSOUND_H
