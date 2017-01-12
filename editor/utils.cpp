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

QString FileDialogUtils::getOpenImageFileName(QWidget* parent, const QString& dir)
{
   return QFileDialog::getOpenFileName(parent, QObject::tr("Choose Image"), dir, FileDialogUtils::ImageFilter);
}

QString FileDialogUtils::getOpenSoundFileName(QWidget* parent, const QString& dir)
{
   return QFileDialog::getOpenFileName(parent, QObject::tr("Choose Sound"), dir, FileDialogUtils::SoundFilter);
}

QString FileDialogUtils::getOpenFontFileName(QWidget* parent, const QString& dir)
{
   return QFileDialog::getOpenFileName(parent, QObject::tr("Choose Font"), dir, FileDialogUtils::FontFilter);
}

QString FileDialogUtils::toFilter(const QString &name, const QStringList &ext)
{
    QStringList formats;
    foreach(const QString& format, ext) {
        formats << "*." + format.toLower();
    }

    return QString("%1 (%2)").arg(name).arg(formats.join(" "));
}
