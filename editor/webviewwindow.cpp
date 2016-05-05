#include "webviewwindow.h"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QStyle>
#include <QApplication>
#include <QDebug>
#include <QWebInspector>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>

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
    QMenu* viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addAction(toolBar->toggleViewAction());
    QMenu* debugMenu = menuBar->addMenu(tr("&Debug"));
    QAction* inspectAction = debugMenu->addAction(tr("Inspect"));
    connect(inspectAction, SIGNAL(triggered()), this, SLOT(showWebInspector()));

    mWebView = new QWebView(this);
    mWebInspector = 0;
    QWebSettings* webSettings = mWebView->settings();
    webSettings->setAttribute(QWebSettings::LocalStorageEnabled, true);
    webSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

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
    if (mWebInspector) {
        mWebInspector->deleteLater();
        mWebInspector = 0;
    }
}

void WebViewWindow::loadWebInspector()
{
    if (! mWebInspector) {
        mWebInspector = new QWebInspector(this);
        mWebInspector->setPage(mWebView->page());
        mWebInspector->setWindowFlags(mWebInspector->windowFlags() | Qt::Window);
        mWebInspector->setWindowModality(Qt::NonModal);
    }
}

void WebViewWindow::showWebInspector()
{
    if (mWebInspector)
        mWebInspector->show();
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
