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


namespace Interaction
{
    enum InputEvent {
        None = 0,
        MousePress = 1,
        MouseRelease = 2,
        MouseMove = 4
    };

    Q_DECLARE_FLAGS(InputEvents, Interaction::InputEvent);

    Q_DECLARE_OPERATORS_FOR_FLAGS(Interaction::InputEvents);
}



#endif // INPUT_EVENTS_H
