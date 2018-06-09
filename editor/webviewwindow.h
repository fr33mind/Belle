#ifndef WEBVIEWWINDOW_H
#define WEBVIEWWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QToolBar>
#include <QLineEdit>

class WebViewWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit WebViewWindow(QWidget *parent = 0);
    void setWebViewSize(int, int);
    void setWebViewWidth(int);
    void setWebViewHeight(int);
    void open(const QUrl&);

protected:
    virtual void closeEvent(QCloseEvent*);
    bool eventFilter(QObject *, QEvent *);

private:
    void loadWebInspector();

private slots:
    void showWebInspector();

signals:

public slots:

private:
   QWebEngineView* mWebView;
   QLineEdit* mAddressBar;
   QToolBar* mToolBar;
   void centerOnScreen();

};

#endif // WEBVIEWWINDOW_H
