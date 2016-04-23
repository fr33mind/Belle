#ifndef WEBVIEWWINDOW_H
#define WEBVIEWWINDOW_H

#include <QMainWindow>
#include <QWebView>
#include <QWebInspector>

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

private:
    void loadWebInspector();

private slots:
    void showWebInspector();

signals:

public slots:

private:
   QWebView* mWebView;
   QWebInspector* mWebInspector;
   void centerOnScreen();

};

#endif // WEBVIEWWINDOW_H
