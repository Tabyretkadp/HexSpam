#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <QPushButton>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <QThread>
#include <vector>
#include <fstream>

#include "../src/include/authinput.h"
#include "./include/tdapp.h"
#include "../src/include/core.h"

TdApp *gTelegramApp = nullptr;

bool code = true;
// bool need_exit = false;

TdApp::TdApp() {
  td::ClientManager::execute(
      td_api::make_object<td_api::setLogVerbosityLevel>(1));
  client_manager_ = std::make_unique<td::ClientManager>();
  client_id_ = client_manager_->create_client_id();
  send_query(td_api::make_object<td_api::getOption>("version"), {});
}

void TdApp::setAuthData(const AuthData &data) { auth_data_ = data; }
void TdApp::setPhoneNumbers(const std::vector<PhoneNumbers> &phone) { phone_numbers_ = phone; }

void TdApp::setLogOut(QTextEdit *log) {
    outLog_ = log;
}

void TdApp::setNeedExit(bool value) {
    need_exit_ = value;
}

void TdApp::setLogFilePath(const QString& path) {
    logFilePath_ = path;
}

void TdApp::checkPhone(const PhoneNumbers &phone_number, std::function<void()> onFinish) {
    QString phone = phone_number.phone;

    auto contact = td_api::make_object<td_api::contact>();
    contact->phone_number_ = phone.toStdString();
    contact->first_name_ = phone.toStdString();
    contact->last_name_ = "Check";

    auto importContact = td_api::make_object<td_api::importContacts>();
    importContact->contacts_.push_back(std::move(contact));

    send_query(std::move(importContact), [this, phone, onFinish](Object object) {
        QString logMessage;
        QString color = "red";

        if (object->get_id() == td_api::error::ID) {
            logMessage = "[ Error ] Что-то точно не так";
        } else if (object->get_id() == td_api::importedContacts::ID) {
            auto result = td::move_tl_object_as<td_api::importedContacts>(object);
            if (!result->user_ids_.empty() && result->user_ids_[0] != 0) {
                color = "green";
                if (!logFilePath_.isEmpty()) {
                    std::ofstream ofs(logFilePath_.toStdString(), std::ios::app);
                    if (ofs.is_open()) {
                        ofs << phone.toStdString() << std::endl;
                    }
                }
            }
            logMessage = QString("[ LOG ] [ user_id: %1 ] %2")
                             .arg(result->user_ids_[0])
                             .arg(phone);
        }

        if (outLog_) {
            QMetaObject::invokeMethod(outLog_, [logMessage, color, log = outLog_]() {
                log->append("<pre style='color:" + color + ";'>" + logMessage + "</pre>");
            }, Qt::QueuedConnection);
        }
        onFinish();
    });
}



void TdApp::loop() {
  while (!need_exit_) {
    if (need_restart_) {
      restart();
    } else if (!are_authorized_) {
      process_response(client_manager_->receive(10));
    } else {
        if (phone_numbers_.empty()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (outLog_) {
                QMetaObject::invokeMethod(outLog_, [log = outLog_]() {
                    log->append("<pre style='color:blue;'>[ LOG ] Все номера обработаны, ждём новых данных...</pre>");
                }, Qt::QueuedConnection);
            }
            need_exit_ = true;
        } else {
            for (const auto& phone : phone_numbers_) {
                std::promise<void> done;
                std::future<void> future = done.get_future();

                checkPhone(phone, [&done]() {
                    done.set_value();
                });

                while (future.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
                    auto response = client_manager_->receive(0);
                    if (response.object) {
                        process_response(std::move(response));
                    }
                }
            }
            while (true) {
                auto response = client_manager_->receive(0);
                if (response.object) {
                    process_response(std::move(response));
                } else {
                    break;
                }
            }
            phone_numbers_.clear();
        }
      }
    }
}

void TdApp::restart() {
  client_manager_.reset();
  *this = TdApp();
}

void TdApp::send_query(td_api::object_ptr<td_api::Function> f,
                       std::function<void(Object)> handler) {
  auto query_id = next_query_id();
  if (handler) {
    handlers_.emplace(query_id, std::move(handler));
  }
  client_manager_->send(client_id_, query_id, std::move(f));
}

void TdApp::process_response(td::ClientManager::Response response) {
  if (!response.object) {
    return;
  }
  if (response.request_id == 0) {
    return process_update(std::move(response.object));
  }
  auto it = handlers_.find(response.request_id);
  if (it != handlers_.end()) {
    it->second(std::move(response.object));
    handlers_.erase(it);
  }
}

std::string TdApp::get_user_name(std::int64_t user_id) {
  auto it = users_.find(user_id);
  if (it == users_.end()) {
    return "unknown user";
  }
  return it->second->first_name_ + " " + it->second->last_name_;
}

std::string TdApp::get_chat_title(std::int64_t chat_id) {
  auto it = chat_title_.find(chat_id);
  if (it == chat_title_.end()) {
    return "unknown chat";
  }
  return it->second;
}

void TdApp::process_update(td_api::object_ptr<td_api::Object> update) {
  td_api::downcast_call(
      *update,
      overloaded(
          [this](td_api::updateAuthorizationState &update_authorization_state) {
            authorization_state_ =
                std::move(update_authorization_state.authorization_state_);
            on_authorization_state_update();
          },
          [this](td_api::updateNewChat &update_new_chat) {
            chat_title_[update_new_chat.chat_->id_] =
                update_new_chat.chat_->title_;
          },
          [this](td_api::updateChatTitle &update_chat_title) {
            chat_title_[update_chat_title.chat_id_] = update_chat_title.title_;
          },
          [this](td_api::updateUser &update_user) {
            auto user_id = update_user.user_->id_;
            users_[user_id] = std::move(update_user.user_);
          },
          [](auto &update) {}));
}

auto TdApp::create_authentication_query_handler() {
  return [this, id = authentication_query_id_](Object object) {
    if (id == authentication_query_id_) {
      check_authentication_error(std::move(object));
    }
  };
}

void TdApp::on_authorization_state_update() {
  authentication_query_id_++;
  td_api::downcast_call(
      *authorization_state_,
      overloaded(
          [this](td_api::authorizationStateReady &) {
            are_authorized_ = true;
            std::cout << "Authorization is completed" << std::endl;
            QMetaObject::invokeMethod(outLog_, [log = outLog_]() {
                log->append("<pre style='color:blue;'>[ LOG ] Authorization is completed</pre>");
            }, Qt::QueuedConnection);
          },
          [this](td_api::authorizationStateLoggingOut &) {
            are_authorized_ = false;
            std::cout << "Logging out" << std::endl;
          },
          [this](td_api::authorizationStateClosing &) {
            std::cout << "Closing" << std::endl;
          },
          [this](td_api::authorizationStateClosed &) {
            are_authorized_ = false;
            need_restart_ = true;
            std::cout << "Terminated" << std::endl;
          },
          [this](td_api::authorizationStateWaitPhoneNumber &) {
            std::cout << "Enter phone number: " << std::flush;
            std::cout << auth_data_.phone.toStdString() << std::endl;
            std::string phone_number = auth_data_.phone.toStdString();
            // std::cin >> phone_number;
            send_query(
                td_api::make_object<td_api::setAuthenticationPhoneNumber>(
                    phone_number, nullptr),
                create_authentication_query_handler());
          },
          [this](td_api::authorizationStateWaitPremiumPurchase &) {
            std::cout << "Telegram Premium subscription is required"
                      << std::endl;
          },
          [this](td_api::authorizationStateWaitEmailAddress &) {
            std::cout << "Enter email address: " << std::flush;
            std::string email_address;
            std::cin >> email_address;
            send_query(
                td_api::make_object<td_api::setAuthenticationEmailAddress>(
                    email_address),
                create_authentication_query_handler());
          },
          [this](td_api::authorizationStateWaitEmailCode &) {
            std::cout << "Enter email authentication code: " << std::flush;
            std::string code;
            std::cin >> code;
            send_query(
                td_api::make_object<td_api::checkAuthenticationEmailCode>(
                    td_api::make_object<td_api::emailAddressAuthenticationCode>(
                        code)),
                create_authentication_query_handler());
          },
          [this](td_api::authorizationStateWaitCode &) {
            std::cout << "authorizationStateWaitCode" << std::endl;
            QThread* thread = QThread::create([=]() {
            while (code) {
                std::cout << "while..." << std::endl;
                if (!auth_data_.code.isEmpty()) {
                code = false;
                std::cout << "Enter authentication code: " << std::flush;
                std::string code = auth_data_.code.toStdString();
                std::cout << auth_data_.code.toStdString() << std::endl;
                // std::cin >> code;
                send_query(
                    td_api::make_object<td_api::checkAuthenticationCode>(code),
                    create_authentication_query_handler());
              } else {
                std::cout << "else" << std::endl;
                code = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
              }
            }
            });
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            thread->start();
          },
          [this](td_api::authorizationStateWaitRegistration &) {
            std::string first_name;
            std::string last_name;
            std::cout << "Enter your first name: " << std::flush;
            std::cin >> first_name;
            std::cout << "Enter your last name: " << std::flush;
            std::cin >> last_name;
            send_query(td_api::make_object<td_api::registerUser>(
                           first_name, last_name, false),
                       create_authentication_query_handler());
          },
          [this](td_api::authorizationStateWaitPassword &) {
              std::cout << auth_data_.password.toStdString() << std::endl;
            if (!auth_data_.password.isEmpty()) {
              std::cout << "Enter authentication password: " << std::flush;
              std::string password = auth_data_.password.toStdString();
              std::cout << auth_data_.password.toStdString() << std::endl;
              // std::getline(std::cin, password);
              send_query(
                  td_api::make_object<td_api::checkAuthenticationPassword>(
                      password),
                  create_authentication_query_handler());
            } else {
            }
          },
          [this](td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
            std::cout << "Confirm this login link on another device: "
                      << state.link_ << std::endl;
          },
          [this](td_api::authorizationStateWaitTdlibParameters &) {
            std::ifstream api_data("../api_data.txt");

            if (!api_data.is_open()) {
              std::cerr << "Ошибка открытия файла api_data.txt!" << std::endl;
            }

            std::string api_id_srt, api_hash;

            if (!std::getline(api_data, api_id_srt)) {
                std::cerr << "Ошибка чтения id!" << std::endl;
            }

            if (!std::getline(api_data, api_hash)) {
                std::cerr << "Ошибка чтения hash!" << std::endl;
            }

            api_data.close();

            int api_id = std::stoi(api_id_srt);

            auto request = td_api::make_object<td_api::setTdlibParameters>();
            request->database_directory_ = "../tdlib";
            request->use_message_database_ = true;
            request->use_secret_chats_ = true;
            request->api_id_ = api_id;
            // request->api_id_ = 23315578;
            request->api_hash_ = api_hash;
            // request->api_hash_ = "14751b0a67ad0d89697e57d03672bce6";
            request->system_language_code_ = "en";
            request->device_model_ = "Desktop";
            request->application_version_ = "1.0";
            send_query(std::move(request),
                       create_authentication_query_handler());
          }));
}

void TdApp::check_authentication_error(Object object) {
  if (object->get_id() == td_api::error::ID) {
    auto error = td::move_tl_object_as<td_api::error>(object);
    std::cout << "Error: " << to_string(error) << std::flush;
    on_authorization_state_update();
  }
}

std::uint64_t TdApp::next_query_id() { return ++current_query_id_; }

void run() {
  TdApp a;
  a.loop();
}
