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

#ifndef NOVEL_PROPERTIES_DIALOG_H
#define NOVEL_PROPERTIES_DIALOG_H

#include <QDialog>

#include "ui_novel_properties_dialog.h"

class NovelPropertiesDialog : public QDialog
{
    Q_OBJECT

    QVariantMap mNovelData;

public:
    explicit NovelPropertiesDialog(QVariantMap& data, QWidget *parent = 0);
    QVariantMap novelData();
    QString enginePath();
    QString browserPath();
    bool useBuiltinBrowser();
    void setEnginePath(const QString&, bool showError=true);

signals:
    
public slots:
    void onWidthChanged(int);
    void onHeightChanged(int);
    void onSizeEdited(const QString&);
    void onTitleEdited(const QString&);
    void onFontFamilyChosen(const QString&);
    void onFontSizeChanged(int);
    void onEnginePathChangeRequest();
    void updateTextSpeedSliderTooltip(int);
    void onBrowserSelect();

private:
    Ui::NovelPropertiesDialog mUi;
};

#endif // NOVEL_PROPERTIES_DIALOG_H
