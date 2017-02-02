#ifndef CHOOSEFONTWIDGET_H
#define CHOOSEFONTWIDGET_H

#include <QComboBox>

class ChooseFontWidget : public QComboBox
{
    Q_OBJECT

public:
    explicit ChooseFontWidget(QWidget *parent = 0);
    void loadFonts();

    static bool showWebSafeFontsMessage();
    static void setShowWebSafeFontsMessage(bool);

protected:
    void loadWebSafeFonts();
    void loadApplicationFonts();
    
signals:
    
private slots:
    void onActivated(int);
    
};

#endif // CHOOSEFONTWIDGET_H
