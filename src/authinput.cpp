#include "./include/authinput.h"
#include "../main/include/tdapp.h"
#include "../main/include/mainwindow.h"

#include <QWidget>
#include <QApplication>
#include <QThread>
#include <iostream>
#include <fstream>

AuthInput::AuthInput(QWidget *parent) : QWidget(parent) {

    // container-start //
    inputData = new QWidget(this);
    inputData->resize(650, 430);
    inputData->setStyleSheet("background: #212021; border-radius: 15px;");

    info = new QLabel(inputData);
    info->setText("Как получить api_id и api_hash:\n\n1. Переходим на сайт: https://my.telegram.org/auth\n2. И все, просто идите по инструкции :)\n3. Дальше вводим это все + номер телефона\n4. Ждем код, который придет в ТГ\n5. Дальше вводим этот код и пароль от ТГ\n\n\nЕсли нет пароля от ТГ:\n\nНужно поставить, так безопаснее\nПоставить можно в настройках:\nКонфиденциальность -> Облачный пароль\n\nКогда все заполнили, нажимаем кнопку 'Создать'");
    info->setStyleSheet("color: #9C9C9B; font-size: 15px;");
    info->move(270, 20);

    line = new QWidget(inputData);
    line->resize(2, 390);
    line->move(240, 20);
    line->setStyleSheet("background: #9C9C9B; border-radius: 15px;");
    // container-end //


    // input-api-start //
    lApi_id = new QLabel("Введите api_id:", inputData);
    lApi_id->move(20, 20);
    lApi_id->setStyleSheet("color: #9C9C9B; font-size: 15px;");

    inApi_id = new QLineEdit(inputData);
    inApi_id->move(20, 50);
    inApi_id->resize(200, 30);
    inApi_id->setStyleSheet("background: #181819; border-radius: 10px; color: #9C9C9B");

    lApi_hash = new QLabel("Введите api_hash:", inputData);
    lApi_hash->move(20, 100);
    lApi_hash->setStyleSheet("color: #9C9C9B; font-size: 15px;");

    inApi_hash = new QLineEdit(inputData);
    inApi_hash->move(20, 130);
    inApi_hash->resize(200, 30);
    inApi_hash->setStyleSheet("background: #181819; border-radius: 10px; color: #9C9C9B");
    // input-api-end //


    // input-phone-start //
    lPhone = new QLabel("Введите Номер:", inputData);
    lPhone->move(20, 180);
    lPhone->setStyleSheet("color: #9C9C9B; font-size: 15px;");

    inPhone = new QLineEdit(inputData);
    inPhone->move(20, 210);
    inPhone->resize(200, 30);
    inPhone->setStyleSheet("background: #181819; border-radius: 10px; color: #9C9C9B");
    // input-phone-end //


    // input-code-start //
    lCode = new QLabel("Введите Код:", inputData);
    lCode->move(20, 260);
    lCode->setStyleSheet("color: #9C9C9B; font-size: 15px;");

    inCode = new QLineEdit(inputData);
    inCode->move(20, 290);
    inCode->resize(200, 30);
    inCode->setStyleSheet("background: #181819; border-radius: 10px; color: #9C9C9B");
    // input-code-end //


    // input-password-start //
    lPass = new QLabel("Введите Пароль:", inputData);
    lPass->move(20, 340);
    lPass->setStyleSheet("color: #9C9C9B; font-size: 15px;");

    inPass = new QLineEdit(inputData);
    inPass->move(20, 370);
    inPass->resize(200, 30);
    inPass->setStyleSheet("background: #181819; border-radius: 10px; color: #9C9C9B");
    // input-password-end //


    // btn-next-start //
    btnNext = new QPushButton("Дальше...", inputData);
    btnNext->move(20, 260);
    btnNext->resize(200, 30);
    btnNext->setStyleSheet("background: #CC2D33; border-radius: 10px; color: white");

    connect(btnNext, &QPushButton::clicked, this, &AuthInput::getBtnData);
    // btn-next-end //


    // btn-create-start //
    btnCreateProfile = new QPushButton("Войти", inputData);
    btnCreateProfile->move(270, 370);
    btnCreateProfile->resize(350, 30);
    btnCreateProfile->setStyleSheet("background: #CC2D33; border-radius: 10px; color: white");

    connect(btnCreateProfile, &QPushButton::clicked, this, &AuthInput::getBtnApi);
    // btn-create-end //


    // hide-start //
    lCode->hide();
    inCode->hide();

    lPass->hide();
    inPass->hide();

    btnCreateProfile->hide();
    // hide-end //

}

AuthData AuthInput::getAuthApi() const {
    return {
        inApi_id->text(),
        inApi_hash->text(),
        inPhone->text(),
        inCode->text(),
        inPass->text(),
    };
}


void AuthInput::getBtnData() {

    std::fstream api_data("../api_data.txt", std::ios::out);

    api_data << inApi_id->text().toInt() << std::endl;;
    api_data << inApi_hash->text().toStdString() << std::endl;

    api_data.close();

    lCode->show();
    inCode->show();

    lPass->show();
    inPass->show();

    btnNext->hide();
    btnCreateProfile->show();

    AuthData data = getAuthApi();

    QThread* thread = QThread::create([=]() {
        gTelegramApp = new TdApp();
        gTelegramApp->setAuthData(data);
        gTelegramApp->loop();
        delete gTelegramApp;
        gTelegramApp = nullptr;
    });

    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();

}

void AuthInput::getBtnApi() {
    std::cout << "getBtnData..." << std::endl;
    if (gTelegramApp) {
        std::cout << "getBtnData OK" << std::endl;
        gTelegramApp->setAuthData(getAuthApi());
    } else {
        qDebug() << "TdApp ещё не запущен!";
    }
}

void AuthInput::resizeEvent(QResizeEvent *event) {
    int x = (width() - inputData->width()) / 2;
    int y = (height() - inputData->height()) / 2;
    inputData->move(x, y);

    QWidget::resizeEvent(event);
}
