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

#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

#include <QString>

class Interaction
{

public:
    enum InputEvent {
        None = 0,
        MousePress = 1,
        MouseRelease = 2,
        MouseMove = 3
    };


    static QString toString(InputEvent ev) {
        switch(ev) {
            //TODO: Rename events
            case None:
                return "None";
            case MousePress:
                return "onMousePress";
            case MouseRelease:
                return "onMouseRelease";
            case MouseMove:
                return "onMouseMove";
        }

        return "";
    }

    Q_DECLARE_FLAGS(InputEvents, InputEvent);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Interaction::InputEvents);

#endif // INPUT_EVENTS_H
