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

#ifndef FADE_H
#define FADE_H

#include "action.h"
#include "fade_editor_widget.h"

class FadeEditorWidget;

class Fade : public Action
{
    Q_OBJECT

public:
    enum Type {
        In,
        Out
    };
    static ActionInfo Info;

public:
    explicit Fade(QObject *parent = 0);
    Fade(const QVariantMap&, QObject *parent = 0);
    static FadeEditorWidget* fadeEditorWidget();
    static void setFadeEditorWidget(FadeEditorWidget*);
    virtual ActionEditorWidget* editorWidget();

    virtual QVariantMap toJsonObject();
    QString displayText() const;

    Fade::Type fadeType();
    QString fadeTypeString() const;
    void setFadeType(Fade::Type);
    void setFadeType(const QString&);
    QString fadeTypeToString(Fade::Type) const;

    double duration() const;
    void setDuration(double);
signals:

public slots:

private:
    void init();

    Fade::Type mFadeType;
    double mDuration;

};

#endif // FADE_H
