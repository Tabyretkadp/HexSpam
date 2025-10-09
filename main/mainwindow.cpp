#include "../src/include/authinput.h"
#include "../src/include/core.h"
#include "./include/mainwindow.h"
#include "./include/tdapp.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  QWidget *central = new QWidget(this);
  setCentralWidget(central);

  QPushButton *btnAuth = new QPushButton("Вход");
  QPushButton *btnCore = new QPushButton("Главная");
  btnAuth->setStyleSheet("color: #9C9C9B;");
  btnCore->setStyleSheet("color: #9C9C9B;");

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(btnAuth);
  buttonLayout->addWidget(btnCore);
  buttonLayout->addStretch();

  QVBoxLayout *vbox = new QVBoxLayout(central);
  vbox->addLayout(buttonLayout);

  stack = new QStackedWidget(central);
  vbox->addWidget(stack);

  AuthInput *auth = new AuthInput();
  Core *core = new Core();
  stack->addWidget(auth);
  stack->addWidget(core);

  connect(btnAuth, &QPushButton::clicked, this,
          [this]() { stack->setCurrentIndex(0); });
  connect(btnCore, &QPushButton::clicked, this,
          [this]() { stack->setCurrentIndex(1); });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setStackIndex(int index) {
  if (stack && index >= 0 && index < stack->count()) {
    stack->setCurrentIndex(index);
  }
}
