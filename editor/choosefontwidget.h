#ifndef CHOOSEFONTWIDGET_H
#define CHOOSEFONTWIDGET_H

#include <QComboBox>
#include <QFontDatabase>
#include <QStyledItemDelegate>

class ChooseFontWidget : public QComboBox
{
    Q_OBJECT

    QFontDatabase mFontDatabase;
    QList<int> mCustomFontsIds;
    int mCurrentIndex;
    QString mCurrentFamily;

public:
    explicit ChooseFontWidget(QWidget *parent = 0);
    void loadFonts();
    bool loadCustomFonts();
    bool loadSystemFonts();
    void addCustomFontItem();
    void setCurrentFontFamily(const QString&);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    
signals:
    void fontChosen(const QString&);
    
public slots:
private slots:
    void onItemActivated(int);
    
};

#endif // CHOOSEFONTWIDGET_H
