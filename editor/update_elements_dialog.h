#ifndef UPDATE_ELEMENTS_DIALOG_H
#define UPDATE_ELEMENTS_DIALOG_H

#include <QDialog>

#include "ui_update_elements_dialog.h"

class UpdateElementsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateElementsDialog(QWidget *parent = 0);
    
signals:
    
public slots:
    
public:
   Ui_UpdateElementsDialog ui;

};

#endif // UPDATE_ELEMENTS_DIALOG_H
