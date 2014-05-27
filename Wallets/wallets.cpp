#include "wallets.hpp"
#include "ui_wallets.h"

#include "../qtreusable/LoginInputBox.hpp"

#include <QAuthenticator>
#include <QNetworkReply>

#ifndef __STATIC_QT
#include <QtWebKitWidgets/QWebView>
#endif

Wallets::Wallets(QWidget* parent) :
QWidget(parent),
ui(new Ui::Wallets),
_url("")
#ifndef __STATIC_QT
,_webView(nullptr)
#endif
{
  ui->setupUi(this);

  ui->labelInfo->setText(tr("Please select wallet"));
  ui->labelInfo->setVisible(true);
  /// Hide the progress bar
  ui->progressBar->setVisible(false);
}

Wallets::Wallets(QWidget* parent, const QString&  url) :
QWidget(parent),
ui(new Ui::Wallets),
_url(url)
#ifndef __STATIC_QT
,_webView(nullptr)
#endif
{
  ui->setupUi(this);

#ifdef __STATIC_QT
  /// QWebView is not available
  ui->labelInfo->setText(tr("No support for Qt WebKit"));
  ui->labelInfo->setVisible(true);
#else
  /// Hide text: "No support for Qt WebKit"
  ui->labelInfo->setVisible(false);
#endif

  /// Hide the progress bar
  ui->progressBar->setVisible(false);
}

Wallets::~Wallets()
{
  delete ui;
}


void Wallets::loadPage()
{
  setupWebPage();
}

void Wallets::setupWebPage()
{
#ifndef __STATIC_QT
  
  if (_webView != nullptr)
  {
    /// Initialization has already been
    return;
  }
   
  /// First time initialization
  _webView = new QWebView(this);

  /// connect signals
  connect(_webView, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()) );
  connect(_webView, SIGNAL(loadProgress(int)), this, SLOT(onLoadProgress(int)));
  connect(_webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
  connect(_webView->page()->networkAccessManager(),
    SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
    this, SLOT(handleAuthenticationRequired(QNetworkReply*, QAuthenticator*)));

  ui->gridLayout->addWidget(_webView, 0, 0, 1, 1);

  _webView->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
  _webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
  _webView->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
  _webView->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false);
  _webView->settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
  _webView->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
  _webView->settings()->setAttribute(QWebSettings::WebGLEnabled, false);

  /*
  webView->settings()->setMaximumPagesInCache(0);
  webView->settings()->setObjectCacheCapacities(0, 0, 0);
  webView->settings()->setOfflineStorageDefaultQuota(0);
  webView->settings()->setOfflineWebApplicationCacheQuota(0);
  webView->settings()->clearIconDatabase();
  webView->settings()->clearMemoryCaches();
  */

  /// load page
  _webView->setUrl(QUrl(_url));

#endif
}

void Wallets::handleAuthenticationRequired(QNetworkReply*, QAuthenticator* authenticator)
{
  QString userName, password;
  LoginInputBox login(this, "Authentication", &userName, &password);
  if (login.exec() == QDialog::Accepted)
  {
    authenticator->setUser(userName);
    authenticator->setPassword(password);
  }
}

void Wallets::onLoadStarted()
{
  ui->progressBar->reset();
  /// Show the progress bar
  ui->progressBar->setVisible(true);  
}

void Wallets::onLoadProgress(int progress)
{
  /** The current value is provided by progress and scales from 0 to 100, 
      which is the default range of QProgressBar.
  */
  ui->progressBar->setValue(progress);
}

void Wallets::onLoadFinished(bool)
{
  /// Hide the progress bar
  ui->progressBar->setVisible(false);
}
