#include "save_project_dialog.h"

#include <QDir>
#include <QFileDialog>

SaveProjectDialog::SaveProjectDialog(const QString& projectName, QWidget *parent) :
    QDialog(parent)
{
    mUi.setupUi(this);

    mProjectName = projectName;
    mUi.projectNameEdit->setText(projectName);
    mProjectParentDirectory = QDir::home();
    mUi.projectDirEdit->setText(mProjectParentDirectory.absoluteFilePath(projectName));

    connect(mUi.projectNameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onProjectNameEdited(const QString&)));
    connect(mUi.projectDirBrowse, SIGNAL(clicked()), this, SLOT(selectDirectory()));
    connect(mUi.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(mUi.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    validateProjectPath();
}

void SaveProjectDialog::onProjectNameEdited(const QString& projectName)
{
    mProjectName = projectName;
    mUi.projectDirEdit->setText(mProjectParentDirectory.absoluteFilePath(projectName));
    validateProjectPath();
}

void SaveProjectDialog::validateProjectPath()
{
    bool valid = true;
    if (mProjectName.isEmpty()) {
        mUi.projectNameEdit->setStyleSheet("background-color: rgba(255, 0, 0, 100);");
        valid = false;
    }
    else
        mUi.projectNameEdit->setStyleSheet("");

    QPushButton* button = mUi.buttonBox->button(QDialogButtonBox::Save);
    if (mProjectParentDirectory.exists(mProjectName)) {
        valid = false;
        mUi.projectDirEdit->setStyleSheet("background-color: rgba(255, 0, 0, 100);");
    }
    else
        mUi.projectDirEdit->setStyleSheet("");

    if (valid)
        button->setDisabled(false);
    else
        button->setDisabled(true);
}

void SaveProjectDialog::selectDirectory()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
    if (path.isEmpty())
        return;

    mProjectParentDirectory.setPath(path);
    mUi.projectDirEdit->setText(mProjectParentDirectory.absoluteFilePath(mProjectName));
    validateProjectPath();
}

QString SaveProjectDialog::projectPath()
{
    return mProjectParentDirectory.absoluteFilePath(mProjectName);
}

QString SaveProjectDialog::projectName()
{
    return mProjectName;
}

QDir SaveProjectDialog::projectParentDirectory()
{
    return mProjectParentDirectory;
}
