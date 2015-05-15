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

#include "aboutdialog.h"

#include <QDebug>
#include <QLabel>
#include <QTextCodec>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    mUi.setupUi(this);

    QString desc = tr("Belle is a cross-platform and easy-to-use visual novel maker,<br/>"
                      "that deploys games for the web.<br/>"
                      "It uses Qt4 for the editor and HTML5 for the engine.");
    QString copyright = "Copyright &copy; 2012-2015 Carlos Pais";
    QString version = "Belle 0.5.1 alpha";

    QString fullDesc = QString("<p><b>%1</b></p><p>%2</p><p>%3</p>").arg(version).arg(desc).arg(copyright);

    QLayout* layout = new QVBoxLayout;
    QLabel* label = new QLabel(fullDesc, mUi.layoutWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    layout->addWidget(label);
    mUi.layoutWidget->setLayout(layout);

    QStringList license;
    license << "This program is free software: you can redistribute it and/or modify"
            << "it under the terms of the GNU General Public License as published by"
            << "the Free Software Foundation, either version 3 of the License, or"
            << "(at your option) any later version."
            << ""
            << "This program is distributed in the hope that it will be useful,"
            << "but WITHOUT ANY WARRANTY; without even the implied warranty of"
            << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
            << "GNU General Public License for more details."
            << ""
            << "You should have received a copy of the GNU General Public License"
            << "along with this program.  If not, see <http://www.gnu.org/licenses/>.";

    mUi.licenseEdit->setText(license.join("\n"));

    QStringList thanksTo;
    QString style = "<style>a {text-decoration: none; }</style>";
    thanksTo    << "Main Developer:"
                << "<a  href='mailto:freemind@live.com.pt'>Carlos Pais</a>"
                << ""
                << "Thanks to:"
                << "<a href='http://qt-project.org'>Qt4</a>"
                << "<a href='http://jquery.com'>jQuery</a>"
                << "<a href='http://buzz.jaysalvat.com/'>Buzz</a>"
                << "<a href='http://www.jstorage.info/'>jStorage</a>"
                << "<a href='https://github.com/ereilin/qt-json'>Eeli Reilin, Luis Gustavo S., Stephen Kockentiedt</a>";

    mUi.creditsBrowser->setOpenExternalLinks(true);
    mUi.creditsBrowser->setHtml(style + thanksTo.join("<br/>"));

    connect(mUi.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    //resize(width(), 400);
}
