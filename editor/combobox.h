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

#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QVariant>
#include <QIcon>
#include <QStyledItemDelegate>
#include <QDialog>

class ComboBoxItemDelegate : public QStyledItemDelegate
{
   Q_OBJECT

   QIcon mDeleteIcon;

public:
    explicit ComboBoxItemDelegate(QObject* parent = 0);
    bool editorEvent (QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

signals:
    void removeItem(int);
};


class ComboBox : public QComboBox
{
    bool mHidePopup;
    QDialog *mDialog;
    QString mDefaultValue;

    Q_OBJECT

public:
    explicit ComboBox(QWidget *parent = 0, const QString& defaultValue="");
    void addItem(const QIcon &icon, const QString &text, const QVariant &userData=QVariant());
    void addItem(const QString &text, const QVariant &userData=QVariant());
    virtual bool eventFilter(QObject *, QEvent *);
    void removeItem(int);
    void setDialog(QDialog*);
    void setDefaultValue(const QString&);
    //bool event(QEvent *event);
    virtual void wheelEvent(QWheelEvent *);

signals:
    void itemRemoved(int);
    void itemRemoved(const QString&);
    void addItemActivated();
    void itemActivated(int);

public slots:
    void onRemoveItem(int);
    void onItemActivated(int);
    void clear();

};


/*class ActionComboBox : public ComboBox
{
    Events::InputEvent mInputEvent;

    public:
        ActionComboBox(Events::InputEvent event, QWidget *parent = 0):
            ComboBox(parent)
        {
            mInputEvent = event;
        }

        Events::InputEvent event()
        {
            return mInputEvent;
        }
};*/

#endif // COMBOBOX_H
