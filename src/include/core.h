#ifndef CORE_H
#define CORE_H

#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QtNetwork/QNetworkProxy>
#include <QRadioButton>
#include <QButtonGroup>

struct PhoneNumbers {
    QString phone;
};

class Core : public QWidget {
  Q_OBJECT
public:
  explicit Core(QWidget *parent = nullptr);

  PhoneNumbers getPhoneNumbers(const std::string &number) const;

  void getAccFolder();
  void getPassFile();
  void updateProxySettings(int proxyType, const QString& address, const QString& portStr, const QString& nameStr, const QString& passStr);

private:
  QWidget *banner;
  QLabel *lBaner;

  QWidget *mainWindow;
  QWidget *placeLog;
  QTextEdit *outLog;

  QLabel *lAccFolder;
  QLineEdit *inAccFolder;
  QPushButton *btnAcceptAccFolder;

  QLabel *lPassFile;
  QLineEdit *inPassFilePath;
  QPushButton *btnAcceptPassFile;

  QWidget* placeProxy;
  QLabel* lproxy;
  QButtonGroup* proxyGroup;
  QRadioButton* noneProxy;
  QRadioButton* inCheckProxySocks5;
  QRadioButton* inCheckProxyHttp;
  QLineEdit* inProxyIP;
  QLineEdit* inProxyPORT;
  QLineEdit* inProxyName;
  QLineEdit* inProxyPass;
  QNetworkProxy* proxy;
};

#endif // CORE_H
