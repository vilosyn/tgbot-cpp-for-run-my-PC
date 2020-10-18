// g++ main.cpp -o telegram_bot -std=c++17 -lTgBot -lboost_system -lssl -lcrypto
// -lpthread
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <tgbot/tgbot.h>

int main() {
  TgBot::Bot bot("bot_id");
  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
    bot.getApi().sendMessage(
        message->chat->id,
        static_cast<std::string>("Команды:\n") +
            "help - вызов данной справки\n\n" +
            "mkdir - создать дирректори/ю. Синтаксис:\n    mkdir "
            "<directory>/<subdirectory>/...\n\n" +
            "save - сохранить файл. Синтаксис:\n    к прикрепляемому файлу "
            "нужно добавить подпись save или save "
            "<directory>/<subdirrectory>/...\n\n" +
            "make - создать исполнимый код, на данный момент работает для: "
            "gcc/g++, clang/clang++, bash, python. Синтаксис:\n    make "
            "<compiller>/<interpreter> <flags> <file(с полными путями до "
            "файлов, т.е. <directory>/<subdirrectory>/.../file)> и т.д. всё "
            "как в терминале, только сначала слово make, Makefile так же можно "
            "собрать, требуется написать make make и путь "
            "<directory>/<subdirrectory>/.../Makefile\n\n" +
            "run - выполнить скомпилированный код, путь к файлу "
            "указывается полностью. Синтаксис:\n    run <run_file> или run "
            "<directory>/<subdirrectory>/.../<run_file>");
  });
  bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
    if (StringTools::startsWith(message->text, "/start")) {
      return;
    }

    std::ofstream bot_out("/home/poesy/project/tgbot/error", std::ios::app);
    bot_out << "Username: " << message->from->username
            << ": wrote: " << message->text << std::endl;
    if (size_t n = message->text.find("help") != std::string::npos) {
      bot.getApi().sendMessage(
          message->chat->id,
          static_cast<std::string>("Команды:\n") +
              "help - вызов данной справки\n\n" +
              "mkdir - создать дирректори/ю. Синтаксис:\n    mkdir "
              "<directory>/<subdirectory>/...\n\n" +
              "save - сохранить файл. Синтаксис:\n    к прикрепляемому файлу "
              "нужно добавить подпись save или save "
              "<directory>/<subdirrectory>/...\n\n" +
              "make - создать исполнимый код, на данный момент работает для: "
              "gcc/g++, clang/clang++, bash, python. Синтаксис:\n    make "
              "<compiller>/<interpreter> <flags> <file(с полными путями до "
              "файлов, т.е. <directory>/<subdirrectory>/.../file)> и т.д. всё "
              "как в терминале, только сначала слово make, Makefile так же "
              "можно собрать, требуется написать make make и путь "
              "<directory>/<subdirrectory>/.../Makefile\n\n" +
              "run - выполнить скомпилированный код, путь к файлу "
              "указывается полностью. Синтаксис:\n    run <run_file> или run "
              "<directory>/<subdirrectory>/.../<run_file>");
    } else if (size_t n = message->text.find("mkdir") != std::string::npos) {
      size_t pos = message->text.find_first_not_of(".~/ ", n + 4);
      std::filesystem::create_directories("/home/poesy/tgbot_projects/" +
                                          message->text.substr(pos));
      bot.getApi().sendMessage(
          message->chat->id, "create directory: " + message->text.substr(pos));
    } else if (size_t n = message->caption.find("save") != std::string::npos) {
      bot_out << "Username: " << message->from->username
              << ": wrote: " << message->caption << std::endl;
      std::shared_ptr<TgBot::File> file =
          bot.getApi().getFile(message->document->fileId);
      size_t pos = message->caption.find_first_not_of(".~/ ", n + 3);
      std::string path =
          message->caption.substr(pos) + "/" + message->document->fileName;
      std::ofstream out_stream("/home/poesy/tgbot_projects/" + path,
                               std::ios::binary);
      out_stream << bot.getApi().downloadFile(file->filePath);
      bot.getApi().sendMessage(message->chat->id, "file saved: " + path);
    } else if (size_t n = message->text.find("make") != std::string::npos) {
      if (message->text.substr(n + 3).find("gcc") != std::string::npos ||
          message->text.substr(n + 3).find("g++") != std::string::npos ||
          message->text.substr(n + 3).find("clang") != std::string::npos ||
          message->text.substr(n + 3).find("make") != std::string::npos ||
          message->text.substr(n + 3).find("bash") != std::string::npos ||
          message->text.substr(n + 3).find("python") != std::string::npos) {
        size_t pos = message->text.find_first_of("gcmbp", n + 3);
        std::string str("cd /home/poesy/tgbot_projects/ && " +
                        message->text.substr(pos) +
                        " &> /home/poesy/tgbot_projects/sys_out");
        std::system(str.c_str());
        if (std::ifstream is{"/home/poesy/tgbot_projects/sys_out",
                             std::ios::binary | std::ios::ate}) {
          auto size = is.tellg();
          std::string str(size, '\0');
          is.seekg(0);
          if (is.read(&str[0], size))
            bot.getApi().sendMessage(message->chat->id, "Out:\n" + str);
        }

        bot.getApi().sendMessage(message->chat->id,
                                 "make command: " + message->text.substr(pos));
      } else {
        bot.getApi().sendMessage(message->chat->id,
                                 "don't make command: " + message->text);
      }
    } else if (size_t n = message->text.find("run") != std::string::npos) {
      if (message->text.substr(0, 3) != message->text) {
        size_t first = message->text.find_first_not_of(".~/ ", n + 2);
        size_t last = message->text.find_last_of("/", std::string::npos);
        std::string str;
        if (last != std::string::npos) {
          str = "cd /home/poesy/tgbot_projects/" +
                message->text.substr(first, last - first) + " && ./" +
                message->text.substr(last + 1) +
                " &> /home/poesy/tgbot_projects/sys_out";
        } else {
          str = "cd /home/poesy/tgbot_projects/ && ./" +
                message->text.substr(first) +
                " &> /home/poesy/tgbot_projects/sys_out";
        }

        std::system(str.c_str());
        if (std::ifstream is{"/home/poesy/tgbot_projects/sys_out",
                             std::ios::binary | std::ios::ate}) {
          auto size = is.tellg();
          std::string str(size, '\0');
          is.seekg(0);
          if (is.read(&str[0], size))
            bot.getApi().sendMessage(message->chat->id, "Out:\n" + str);
        }

        bot.getApi().sendMessage(message->chat->id,
                                 "run command: " + message->text.substr(first));
      } else {
        bot.getApi().sendMessage(message->chat->id,
                                 "don't run command: " + message->text);
      }
    } else {
      bot.getApi().sendMessage(message->chat->id,
                               "not found command: " + message->text);
    }
  });
  try {
    std::ofstream bot_out("/home/poesy/project/tgbot/error", std::ios::app);
    bot_out << "Bot username: " << bot.getApi().getMe()->username << std::endl;
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      // bot_out << "Long poll started" << std::endl;
      longPoll.start();
    }
  } catch (TgBot::TgException &e) {
    std::ofstream bot_out("/home/poesy/project/tgbot/error", std::ios::app);
    bot_out << "error: " << e.what() << std::endl;
  }

  return 0;
}
