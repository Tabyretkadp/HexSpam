#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include "../../src/include/authinput.h"
#include "../../src/include/core.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace detail {
template <class... Fs> struct overload;

template <class F> struct overload<F> : public F {
  explicit overload(F f) : F(f) {}
};
template <class F, class... Fs>
struct overload<F, Fs...> : public overload<F>, public overload<Fs...> {
  overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {}
  using overload<F>::operator();
  using overload<Fs...>::operator();
};
}

template <class... F> auto overloaded(F... f) {
  return detail::overload<F...>(f...);
}

namespace td_api = td::td_api;

class TdApp {
public:
  TdApp();
  void checkPhone(const PhoneNumbers &phone_number, std::function<void()> onFinish);
  void loop();

  void setAuthData(const AuthData &data);
  void setPhoneNumbers(const std::vector<PhoneNumbers> &phone);
  void setLogOut(QTextEdit *log);
  void setLogFilePath(const QString& path);

  void setNeedExit(bool value);

private:
  bool need_exit_ = false;

  AuthData auth_data_;

  std::vector<PhoneNumbers> phone_numbers_;
  QTextEdit *outLog_ = nullptr;
  QString logFilePath_;

private:
  using Object = td_api::object_ptr<td_api::Object>;
  std::unique_ptr<td::ClientManager> client_manager_;
  std::int32_t client_id_{0};

  td_api::object_ptr<td_api::AuthorizationState> authorization_state_;
  bool are_authorized_{false};
  bool need_restart_{false};
  std::uint64_t current_query_id_{0};
  std::uint64_t authentication_query_id_{0};

  std::map<std::uint64_t, std::function<void(Object)>> handlers_;

  std::map<std::int64_t, td_api::object_ptr<td_api::user>> users_;

  std::map<std::int64_t, std::string> chat_title_;

private:
  void restart();
  void send_query(td_api::object_ptr<td_api::Function> f,
                  std::function<void(Object)> handler);
  void process_response(td::ClientManager::Response response);
  std::string get_user_name(std::int64_t user_id);
  std::string get_chat_title(std::int64_t chat_id);
  void process_update(td_api::object_ptr<td_api::Object> update);
  auto create_authentication_query_handler();
  void on_authorization_state_update();
  void check_authentication_error(Object object);
  std::uint64_t next_query_id();
};

void run();

extern TdApp *gTelegramApp;
