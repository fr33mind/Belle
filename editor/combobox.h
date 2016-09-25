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
#include <QStandardItemModel>

#define INVALID_DATA "__INVALID__"

class QStandardItemModel;

class ComboBoxItemDelegate : public QStyledItemDelegate
{
   Q_OBJECT

   QIcon mDeleteIcon;
   QStyleOptionViewItem mLastStyleOption;
   QModelIndex mLastIndex;

public:
    ComboBoxItemDelegate(QObject* parent, QComboBox* cmb);
    virtual bool editorEvent (QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const;
    bool mouseReleased(QEvent*);

    static bool isSeparator(const QModelIndex &index) {
        return index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator");
    }
    static void setSeparator(QAbstractItemModel *model, const QModelIndex &index) {
        model->setData(index, QString::fromLatin1("separator"), Qt::AccessibleDescriptionRole);
        if (QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model))
            if (QStandardItem *item = m->itemFromIndex(index))
                item->setFlags(item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
    }

signals:
    void removeItem(int);

private:
    QStyleOptionMenuItem getStyleOption(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const;
    QComboBox *mCombo;
};


class ComboBox : public QComboBox
{
    QString mDefaultText;
    ComboBoxItemDelegate* mItemDelegate;

    Q_OBJECT

public:
    explicit ComboBox(QWidget *parent = 0, const QString& defaultText="");
    void addItem(const QIcon &icon, const QString &text, const QVariant &userData=QVariant());
    void addItem(const QString &text, const QVariant &userData=QVariant());
    virtual bool eventFilter(QObject *, QEvent *);
    void removeItem(int);
    void setDefaultText(const QString&);
    QString defaultText() const;
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

#endif // COMBOBOX_H
