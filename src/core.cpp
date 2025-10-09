#include "./include/core.h"
#include "../main/include/tdapp.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QThread>

#include <fstream>
#include <iostream>

QString pathToAcc = "";
QString pathToPass = "";
static bool isChecking = false;

bool isProxySocks5 = false;

Core::Core(QWidget *parent) : QWidget(parent) {

  banner = new QWidget(this);
  banner->resize(650, 130);
  banner->setStyleSheet("background: #212021; border-radius: 15px;");

  QVBoxLayout *vboxLbanner = new QVBoxLayout(banner);

  lBaner = new QLabel(banner);
  lBaner->setText("ТЕЛЕГРАМ ЧЕКЕР НОМЕРОВ");
  lBaner->setStyleSheet("color: #9C9C9B; font-size: 15px;");
  lBaner->setAlignment(Qt::AlignCenter);

  vboxLbanner->addWidget(lBaner);

  banner->setLayout(vboxLbanner);

  mainWindow = new QWidget(this);
  mainWindow->move(0, 150);
  mainWindow->resize(650, 575);
  mainWindow->setStyleSheet(
      "background: #191919; border-radius: 15px; border: 2px solid #212021;");

  placeLog = new QWidget(this);
  placeLog->move(660, 0);
  placeLog->resize(520, 725);
  placeLog->setStyleSheet(
      "background: #191919; border-radius: 0px; border: 2px solid #212021;");

  outLog = new QTextEdit(placeLog);
  outLog->resize(520, 725);
  outLog->setReadOnly(true);

  ///

  // input-acc-start //

  lAccFolder = new QLabel(mainWindow);
  lAccFolder->setText("Введите путь к логам: ");
  lAccFolder->move(10, 20);
  lAccFolder->setStyleSheet("color: #9C9C9B; border: none; font-size: 15px;");

  inAccFolder = new QLineEdit(mainWindow);
  inAccFolder->move(10, 50);
  inAccFolder->resize(630, 35);
  inAccFolder->setStyleSheet(
      "background: #212021; border-radius: 10px; color: #9C9C9B;");

  // input-acc-end //


  // input-pass-file-start //

  lPassFile = new QLabel(mainWindow);
  lPassFile->setText("Введите путь к номерам: ");
  lPassFile->move(10, 110);
  lPassFile->setStyleSheet("color: #9C9C9B; border: none; font-size: 15px;");

  inPassFilePath = new QLineEdit(mainWindow);
  inPassFilePath->move(10, 140);
  inPassFilePath->resize(630, 35);
  inPassFilePath->setStyleSheet(
      "background: #212021; border-radius: 10px; color: #9C9C9B;");

  btnAcceptPassFile = new QPushButton("Сохранить", mainWindow);
  btnAcceptPassFile->move(530, 500);
  btnAcceptPassFile->resize(100, 35);
  btnAcceptPassFile->setStyleSheet(
      "background: #CC2D33; border-radius: 10px; color: white");

  connect(btnAcceptPassFile, &QPushButton::clicked, this, &Core::getPassFile);

  // input-pass-file-start //


  // proxy-start //

  proxy = new QNetworkProxy();

  placeProxy = new QWidget(mainWindow);
  placeProxy->resize(630, 270);
  placeProxy->move(10, 200);

  lproxy = new QLabel(placeProxy);
  lproxy->setText("Настройка прокси:");
  lproxy->setStyleSheet("color: #9C9C9B; border: none; font-size: 15px;");
  lproxy->move(10, 10);

  proxyGroup = new QButtonGroup(placeProxy);

  noneProxy = new QRadioButton("Без прокси", placeProxy);
  noneProxy->setStyleSheet("color: #9C9C9B; border: none; font-size: 15px;");
  noneProxy->move(10, 40);
  proxyGroup->addButton(noneProxy, 0);

  inCheckProxySocks5 = new QRadioButton("Socks5", placeProxy);
  inCheckProxySocks5->setStyleSheet("color: #9C9C9B; border: none; font-size: 15px;");
  inCheckProxySocks5->move(10, 70);
  proxyGroup->addButton(inCheckProxySocks5, 1);

  inCheckProxyHttp = new QRadioButton("Http", placeProxy);
  inCheckProxyHttp->setStyleSheet("color: #9C9C9B; border: none; font-size: 15px;");
  inCheckProxyHttp->move(10, 100);
  proxyGroup->addButton(inCheckProxyHttp, 2);

  inProxyIP = new QLineEdit("Адрес", placeProxy);
  inProxyIP->resize(150, 30);
  inProxyIP->move(10, 140);
  inProxyIP->setStyleSheet(
      "background: #212021; border-radius: 10px; color: #9C9C9B;");

  inProxyPORT = new QLineEdit("Порт", placeProxy);
  inProxyPORT->resize(100, 30);
  inProxyPORT->move(170, 140);
  inProxyPORT->setStyleSheet(
      "background: #212021; border-radius: 10px; color: #9C9C9B;");

  inProxyName = new QLineEdit("Логин", placeProxy);
  inProxyName->resize(260, 30);
  inProxyName->move(10, 180);
  inProxyName->setStyleSheet(
      "background: #212021; border-radius: 10px; color: #9C9C9B;");

  inProxyPass = new QLineEdit("пароль", placeProxy);
  inProxyPass->resize(260, 30);
  inProxyPass->move(10, 220);
  inProxyPass->setStyleSheet(
      "background: #212021; border-radius: 10px; color: #9C9C9B;");

  // connect(btnAcceptPassFile, &QPushButton::clicked, this, [this](int id){
  //     updateProxySettings(proxyGroup->checkedId(), inProxyIP->text(), inProxyPORT->text(), inProxyName->text(), inProxyPass->text());
  //     getPassFile();
  // });

  // connect(proxyGroup, &QButtonGroup::idClicked,
  //         [this](int id) {
  //             updateProxySettings(id, inProxyIP->text(), inProxyPORT->text(), inProxyName->text(), inProxyPass->text());
  // });

  // proxy->setType(QNetworkProxy::Socks5Proxy);
  // proxy->setHostName("proxy.example.com");
  // proxy->setPort(1080);
  // proxy->setUser("username");
  // proxy->setPassword("password");
  // QNetworkProxy::setApplicationProxy(*proxy);

  // proxy-start //
}

void Core::updateProxySettings(int proxyType, const QString& address, const QString& portStr, const QString& nameStr, const QString& passStr)
{
    switch(proxyType) {
    case 0:
        proxy->setType(QNetworkProxy::NoProxy);
        break;
    case 1:
        proxy->setType(QNetworkProxy::Socks5Proxy);
        proxy->setHostName(address);
        proxy->setPort(portStr.toInt());
        proxy->setUser(nameStr);
        proxy->setPassword(passStr);
        break;
    case 2:
        proxy->setType(QNetworkProxy::HttpProxy);
        proxy->setHostName(address);
        proxy->setPort(portStr.toInt());
        proxy->setUser(nameStr);
        proxy->setPassword(passStr);
        break;
    }

    QNetworkProxy::setApplicationProxy(*proxy);
}

PhoneNumbers Core::getPhoneNumbers(const std::string &number) const {
    PhoneNumbers p;
    p.phone = QString::fromStdString(number);
    return p;
}

void Core::getAccFolder() {
  pathToAcc = inAccFolder->text();
  std::ofstream logs(pathToAcc.toStdString());
  if(!logs.is_open()) {
      outLog->append("<pre style='color:red;'>[ :( ] Что-то не так с путем к логам</pre>");
      return;
  }
  logs.close();
}

void Core::getPassFile() {
    // outLog->append("<pre style='color:pink;'>:3</pre>");r
    if (isChecking) {
        outLog->append("<pre style='color:orange;'>[ ! ] Проверка уже запущена, дождитесь завершения</pre>");
        return;
    }

    pathToPass = inPassFilePath->text();
    std::ifstream acc(pathToPass.toStdString());
    if (!acc.is_open()) {
        outLog->append("<pre style='color:red;'>[ :( ] Что-то не так с путем к номерам</pre>");
        return;
    }

    getAccFolder();

    std::vector<PhoneNumbers> phones;
    std::string number;
    while(std::getline(acc, number)) {
        phones.push_back(getPhoneNumbers(number));
    }

    QTextEdit* logOutput = outLog;

    QThread* thread = new QThread;
    QObject* worker = new QObject;

    connect(thread, &QThread::started, [phones, logOutput, thread, worker]() {
        TdApp* app = new TdApp;
        app->setLogOut(logOutput);
        app->setPhoneNumbers(phones);
        app->setLogFilePath(pathToAcc);
        app->loop();
        delete app;
        thread->quit();
    });

    connect(thread, &QThread::finished, [=]() {
        isChecking = false;
    });

    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    isChecking = true;
    thread->start();
}


