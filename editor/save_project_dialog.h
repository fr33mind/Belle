#ifndef SAVE_PROJECT_DIALOG_H
#define SAVE_PROJECT_DIALOG_H

#include "ui_save_project_dialog.h"

#include <QDialog>
#include <QDir>


class SaveProjectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SaveProjectDialog(const QString& projectName="", QWidget *parent = 0);
    QString projectPath();
    QDir projectParentDirectory();
    QString projectName();
    
signals:
    
private slots:
    void onProjectNameEdited(const QString&);
    void selectDirectory();

private:
    void validateProjectPath();
    Ui::SaveProjectDialog mUi;
    QDir mProjectParentDirectory;
    QString mProjectName;
    
};

#endif // SAVE_PROJECT_DIALOG_H
