#include "fonteditorwidget.h"

#include "font.h"
#include "fontlibrary.h"

FontEditorWidget::FontEditorWidget(QWidget *parent) :
    GameObjectEditorWidget(parent)
{
    mFileComboBox = new FileComboBox(FileComboBox::FontFilter, this);
    mFontFamilyEdit = new QLineEdit(this);
    mFontFamilyEdit->setReadOnly(true);
    mFontWeightsComboBox = new QComboBox(this);
    setupFontWeights(mFontWeightsComboBox);
    mFontStylesComboBox = new QComboBox(this);
    setupFontStyles(mFontStylesComboBox);
    mFallbackEdit = new QLineEdit(this);
    mFallbackEdit->setPlaceholderText("Arial, Helvetica, sans-serif");
    mGenericFontFamiliesComboBox = new QComboBox(this);
    setupGenericFontFamilies(mGenericFontFamiliesComboBox);

    beginGroup(tr("Font"));
    appendRow(tr("Files"), mFileComboBox);
    appendRow(tr("Font family"), mFontFamilyEdit);
    appendRow(tr("Font weight"), mFontWeightsComboBox);
    appendRow(tr("Font style"), mFontStylesComboBox);
    appendRow(tr("Fallback"), mFallbackEdit);
    appendRow(tr("Generic Font Family"), mGenericFontFamiliesComboBox);
    endGroup();

    connect(mFileComboBox, SIGNAL(fileAdded(const QString&)), this, SLOT(onFileAdded(const QString&)));
    connect(mFileComboBox, SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
    connect(mFontWeightsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontWeightChanged(int)));
    connect(mFontStylesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontStylesChanged(int)));
    connect(mFallbackEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onFallbackChanged(const QString&)));
    connect(mGenericFontFamiliesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onGenericFontFamilyChanged(int)));

    resizeColumnToContents(0);
}

void FontEditorWidget::updateData(GameObject* obj)
{
    GameObjectEditorWidget::updateData(obj);
    Font* font = qobject_cast<Font*>(obj);
    if (! font)
        return;

    FontAsset* fontAsset = font->asset();
    if (!fontAsset)
        return;

    mFileComboBox->clear();

    if (fontAsset) {
        foreach(Asset* asset, fontAsset->sources()) {
            mFileComboBox->addItem(GameObjectMetaType::icon(GameObjectMetaType::Font), asset->name(), asset->path());
        }
    }

    mFontFamilyEdit->setText(fontAsset->fontFamily());

    int index = mFontWeightsComboBox->findData(fontAsset->fontWeight());
    mFontWeightsComboBox->setCurrentIndex(index);

    index = mFontStylesComboBox->findData(fontAsset->fontStyle());
    mFontStylesComboBox->setCurrentIndex(index);

    mFallbackEdit->setText(fontAsset->fallback());

    index = mGenericFontFamiliesComboBox->findData(fontAsset->genericFontFamily());
    mGenericFontFamiliesComboBox->setCurrentIndex(index);
}

FontAsset* FontEditorWidget::currentFontAsset() const
{
    Font* font = qobject_cast<Font*>(mGameObject);
    if (!font)
        return 0;

    return font->asset();
}

void FontEditorWidget::onFileAdded(const QString & path)
{
    Font* font = qobject_cast<Font*>(mGameObject);
    if (!font)
        return;

    font->addFile(path);
}

void FontEditorWidget::onItemRemoved(int index)
{
    FontAsset* fontAsset = currentFontAsset();
    if (fontAsset)
        fontAsset->removeSourceAt(index);
}

void FontEditorWidget::onFontWeightChanged(int index)
{
    FontAsset* fontAsset = currentFontAsset();
    if (fontAsset)
        fontAsset->setFontWeight(mFontWeightsComboBox->itemData(index).toString());
}

void FontEditorWidget::setupFontWeights(QComboBox * comboBox)
{
    QList<int> weights = FontLibrary::fontWeights();

    comboBox->clear();
    foreach(int weight, weights) {
        comboBox->addItem(FontLibrary::fontWeightName(weight), FontLibrary::cssFontWeight(weight));
    }
}

void FontEditorWidget::onFontStylesChanged(int index)
{
    FontAsset* fontAsset = currentFontAsset();
    if (fontAsset)
        fontAsset->setFontStyle(mFontStylesComboBox->itemData(index).toString());
}

void FontEditorWidget::setupFontStyles(QComboBox * comboBox)
{
    QList<QFont::Style> styles = FontLibrary::fontStyles();

    comboBox->clear();
    foreach(QFont::Style style, styles) {
        comboBox->addItem(FontLibrary::fontStyleName(style), FontLibrary::cssFontStyle(style));
    }
}

void FontEditorWidget::onFallbackChanged(const QString & text)
{
    FontAsset* fontAsset = currentFontAsset();
    if (fontAsset) {
        fontAsset->setFallback(text);
    }
}

void FontEditorWidget::setupGenericFontFamilies(QComboBox * comboBox)
{
    QList<QFont::StyleHint> families = FontLibrary::genericFontFamilies();

    comboBox->clear();
    comboBox->addItem("", "");
    foreach(QFont::StyleHint family, families) {
        comboBox->addItem(FontLibrary::genericFontFamilyName(family), FontLibrary::cssGenericFontFamily(family));
    }
}

void FontEditorWidget::onGenericFontFamilyChanged(int index)
{
    FontAsset* fontAsset = currentFontAsset();
    if (fontAsset)
        fontAsset->setGenericFontFamily(mGenericFontFamiliesComboBox->itemData(index).toString());
}
