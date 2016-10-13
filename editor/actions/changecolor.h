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

#ifndef CHANGECOLOR_H
#define CHANGECOLOR_H

#include "action.h"

class ChangeColor : public Action
{
    Q_OBJECT

public:
    explicit ChangeColor(QObject *parent = 0);
    ChangeColor(const QVariantMap&, QObject *parent = 0);
    virtual ~ChangeColor();
    virtual QVariantMap toJsonObject(bool internal=true) const;
    virtual QString displayText() const;

    QColor color() const;
    void setColor(const QColor&);

    void setColorChangeEnabled(bool);
    bool isColorChangeEnabled() const;

    void setImage(const QString&);
    void setImage(ImageFile*);
    ImageFile* image() const;

    void setImageChangeEnabled(bool);
    bool isImageChangeEnabled() const;

    int opacity() const;
    void setOpacity(int);

    void setOpacityChangeEnabled(bool);
    bool isOpacityChangeEnabled() const;

protected:
    virtual void loadSceneObject();
    virtual void restoreSceneObject();

private:
    void _loadSceneObject();

signals:
    
public slots:

private:
    QColor mColor;
    ImageFile* mImage;
    bool mReleaseImage;
    bool mImageChangeEnabled;
    bool mColorChangeEnabled;
    bool mOpacityChangeEnabled;

    void init();
    void releaseImage();
};

#endif // CHANGECOLOR_H
