#include "webviewwindow.h"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QStyle>
#include <QApplication>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QWebEngineSettings>

WebViewWindow::WebViewWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QAction* reloadAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_BrowserReload), tr("Reload"), this);
    reloadAction->setShortcut(QKeySequence(QKeySequence::Refresh));

    QToolBar* toolBar = new QToolBar(tr("ToolBar"), this);
    mAddressBar = new QLineEdit(toolBar);
    mAddressBar->setReadOnly(true);
    toolBar->addWidget(mAddressBar);
    toolBar->addAction(reloadAction);
    toolBar->setMovable(false);
    toolBar->hide();
    toolBar->installEventFilter(this);
    mToolBar = toolBar;
    addToolBar(toolBar);

    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(reloadAction);
    QAction* quitAction = fileMenu->addAction(QIcon(":/media/application_exit.png"), tr("Quit"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    QMenu* viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addAction(toolBar->toggleViewAction());

    mWebView = new QWebEngineView(this);
    QWebEngineSettings* webSettings = mWebView->settings();
    webSettings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    #if QT_VERSION >= 0x050700
        webSettings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
    #endif

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mWebView);
    layout->setAlignment(mWebView, Qt::AlignCenter);
    setCentralWidget(centralWidget);
    setWindowModality(Qt::WindowModal);

    connect(reloadAction, SIGNAL(triggered()), mWebView, SLOT(reload()));

    loadWebInspector();
}

void WebViewWindow::setWebViewSize(int w, int h)
{
    setWebViewWidth(w);
    setWebViewHeight(h);
}

void WebViewWindow::setWebViewWidth(int w)
{
    mWebView->setFixedWidth(w);
}

void WebViewWindow::setWebViewHeight(int h)
{
    mWebView->setFixedHeight(h);
}

void WebViewWindow::open(const QUrl& url)
{
    show();
    mWebView->show();
    loadWebInspector();
    centerOnScreen();

    if (mWebView->url() == url)
        mWebView->reload();
    else
        mWebView->setUrl(url);
    mAddressBar->setText(url.toString());
}

void WebViewWindow::closeEvent(QCloseEvent *event)
{
    mWebView->setUrl(QUrl());
    mWebView->hide();
}

void WebViewWindow::loadWebInspector()
{
}

void WebViewWindow::showWebInspector()
{
}

void WebViewWindow::centerOnScreen()
{
    QDesktopWidget* desktopWidget = QApplication::desktop();

    if (desktopWidget) {
        QWidget* mainWindow = qobject_cast<QWidget*>(parent());
        QRect geometry = desktopWidget->screenGeometry(mainWindow);

        int centerWidth = geometry.width()/2 - width()/2;
        if (centerWidth < 0)
            centerWidth = 0;
        int centerHeight = geometry.height()/2 - height()/2;
        if (centerHeight < 0)
            centerHeight = 0;

        //in case of multi-monitor
        centerWidth += geometry.x();
        centerHeight += geometry.y();

        move(centerWidth, centerHeight);
    }
}

bool WebViewWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == mToolBar && event->type() == QEvent::HideToParent)
        adjustSize();
    return QMainWindow::eventFilter(obj, event);
}
