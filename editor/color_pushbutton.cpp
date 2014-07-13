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

#include "color_pushbutton.h"

#include <QColorDialog>
#include <QDebug>
#include <QMessageBox>

ColorPushButton::ColorPushButton(QWidget *parent, const QColor &color) :
    QPushButton(parent)
{
    setColor(color);
    connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void ColorPushButton::onClick()
{
    if (mColor.isValid()) {
        QMessageBox msgBox(QMessageBox::NoIcon, tr("What do you want to do?"), "");
        QPushButton * chooseButton = new QPushButton(tr("Choose new color"), &msgBox);
        QPushButton * removeButton = new QPushButton(tr("Remove current color"), &msgBox);
        msgBox.addButton(chooseButton, QMessageBox::AcceptRole);
        msgBox.addButton(removeButton, QMessageBox::ResetRole);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(chooseButton);
        msgBox.exec();

        if (msgBox.clickedButton() == removeButton) {
            setColor(QColor());
        }

        if (msgBox.clickedButton() != chooseButton)
            return;
    }

    QColorDialog dialog(mColor);
    int accepted = dialog.exec();

    if (accepted) {
        QColor color = dialog.currentColor();
        setColor(color);
    }
}

void ColorPushButton::setColor(const QColor& color)
{
    QString name = tr("None");
    if (color.isValid()) {
        QString styleSheet = QString("QPushButton{ background-color: rgb(%1, %2, %3);}").arg(QString::number(color.red()))
                                                                          .arg(QString::number(color.green()))
                                                                          .arg(QString::number(color.blue()));
        setStyleSheet(styleSheet);
        name = color.name();
    }
    else {
        if (! styleSheet().isEmpty())
            setStyleSheet("");
    }

    mColor = color;
    this->setText(name);
    emit colorChosen(mColor);
}

QColor ColorPushButton::color()
{
    return mColor;
}
