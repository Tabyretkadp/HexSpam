#ifndef AUTHINPUT_H
#define AUTHINPUT_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

struct AuthData {
  QString api_id;
  QString api_hash;
  QString phone;
  QString code;
  QString password;
};

class AuthInput : public QWidget {
  Q_OBJECT
public:
  explicit AuthInput(QWidget *parent = nullptr);
  void resizeEvent(QResizeEvent *event);

  AuthData getAuthApi() const;
  AuthData getAuthData() const;

  void getBtnApi();
  void getBtnData();

private:
  QWidget *inputData;

  QLabel *info;

  QWidget *line;

  QLabel *lApi_id;
  QLineEdit *inApi_id;

  QLabel *lApi_hash;
  QLineEdit *inApi_hash;

  QLabel *lPhone;
  QLineEdit *inPhone;

  QLabel *lCode;
  QLineEdit *inCode;

  QLabel *lPass;
  QLineEdit *inPass;

  QPushButton *btnCreateProfile;
  QPushButton *btnNext;
};

#endif // AUTHINPUT_H
