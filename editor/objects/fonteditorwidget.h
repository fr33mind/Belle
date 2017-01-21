#ifndef FONTEDITORWIDGET_H
#define FONTEDITORWIDGET_H

#include "gameobject_editorwidget.h"
#include "filecombobox.h"
#include "fontasset.h"

class FontAsset;

class FontEditorWidget : public GameObjectEditorWidget
{
    Q_OBJECT
public:
    explicit FontEditorWidget(QWidget *parent = 0);

protected:
    virtual void updateData(GameObject*);
    FontAsset* currentFontAsset() const;

signals:

private slots:
    void onFileAdded(const QString&);
    void onItemRemoved(int);
    void onFontWeightChanged(int);
    void onFontStylesChanged(int);
    void onFallbackChanged(const QString&);
    void onGenericFontFamilyChanged(int);

private:
    FileComboBox* mFileComboBox;
    QLineEdit* mFontFamilyEdit;
    QComboBox* mFontWeightsComboBox;
    QComboBox* mFontStylesComboBox;
    QLineEdit* mFallbackEdit;
    QComboBox* mGenericFontFamiliesComboBox;

    void setupFontWeights(QComboBox*);
    void setupFontStyles(QComboBox*);
    void setupGenericFontFamilies(QComboBox*);
    void updateFileComboBoxFirstItem();

};

#endif // FONTEDITORWIDGET_H
