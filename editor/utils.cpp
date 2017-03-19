#include "utils.h"
#include "imagefile.h"
#include "soundasset.h"
#include "fontasset.h"
#include <QFileDialog>

const QString FileDialogUtils::ImageFilter = FileDialogUtils::toFilter(QObject::tr("Images"),
                                                                       ImageFile::supportedFormats());
const QString FileDialogUtils::SoundFilter = FileDialogUtils::toFilter(QObject::tr("Sounds"),
                                                                       SoundAsset::supportedFormats());
const QString FileDialogUtils::FontFilter = FileDialogUtils::toFilter(QObject::tr("Fonts"),
                                                                       FontAsset::supportedFormats());
const QString FileDialogUtils::AllFilesFilter = "All Files (*)";

QString FileDialogUtils::getOpenImageFileName(QWidget* parent, const QString& dir, bool allFilesFilter)
{
   QString filters = FileDialogUtils::buildFilters(FileDialogUtils::ImageFilter, allFilesFilter);
   return QFileDialog::getOpenFileName(parent, QObject::tr("Choose Image"), dir, filters);
}

QString FileDialogUtils::getOpenSoundFileName(QWidget* parent, const QString& dir, bool allFilesFilter)
{
   QString filters = FileDialogUtils::buildFilters(FileDialogUtils::SoundFilter, allFilesFilter);
   return QFileDialog::getOpenFileName(parent, QObject::tr("Choose Sound"), dir, filters);
}

QString FileDialogUtils::getOpenFontFileName(QWidget* parent, const QString& dir, bool allFilesFilter)
{
   QString filters = FileDialogUtils::buildFilters(FileDialogUtils::FontFilter, allFilesFilter);
   return QFileDialog::getOpenFileName(parent, QObject::tr("Choose Font"), dir, filters);
}

QString FileDialogUtils::toFilter(const QString &name, const QStringList &ext)
{
    QStringList formats;
    foreach(const QString& format, ext) {
        formats << "*." + format.toLower();
    }

    return QString("%1 (%2)").arg(name).arg(formats.join(" "));
}

QString FileDialogUtils::buildFilters(const QString& filter, bool allFilesFilter)
{
    QStringList filters;
    filters << filter;
    if (allFilesFilter)
        filters << FileDialogUtils::AllFilesFilter;
    return filters.join(";;");
}
