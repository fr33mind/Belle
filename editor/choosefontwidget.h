#ifndef CHOOSEFONTWIDGET_H
#define CHOOSEFONTWIDGET_H

#include <QComboBox>

class ChooseFontWidget : public QComboBox
{
    Q_OBJECT

public:
    explicit ChooseFontWidget(QWidget *parent = 0);
    void loadFonts();

protected:
    void loadWebSafeFonts();
    void loadApplicationFonts();
    
signals:
    
private slots:
    
};

#endif // CHOOSEFONTWIDGET_H
