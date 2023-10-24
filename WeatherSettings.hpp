#pragma once

#include <boost/json/src.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
namespace bj = boost::json;

class WeatherSettings {
 public:
  // WeatherSettings(std::string zipCode, int delay, int retry, int periods)
  //     : zipCode(zipCode), delay(delay), retry(retry), periods(periods) {}
  WeatherSettings() {
    const fs::path exePath = getExecuatablePath();
    settings_file = exePath.parent_path() / "settings.json";
  }
  std::string getZipCode() { return zipCode; }
  int getDelay() { return delay; }
  int getRetry() { return retry; }
  int getPeriods() { return periods; }
  std::string getForecastAPI() { return forecastAPI; }
  std::string getAlertsAPI() { return alertsAPI; }
  std::string getCity() { return city; }
  std::string getState() { return state; }
  void setZipCode(std::string passedZipCode) { zipCode = passedZipCode; }
  void setDelay(int passedDelay) { delay = passedDelay; }
  void setRetry(int passedRetry) { retry = passedRetry; }
  void setForecastPeriods(int passedPeriods) { periods = passedPeriods; }
  void setForecastAPI(std::string input) { forecastAPI = input; }
  void setAlertsAPI(std::string input) { alertsAPI = input; }
  void setCity(std::string input) { city = input; }
  void setState(std::string input) { state = input; }
  void saveSettings() {
    bj::object obj;
    obj["zipCode"] = zipCode;
    obj["delay"] = delay;
    obj["retry"] = retry;
    obj["periods"] = periods;
    obj["forecastAPI"] = forecastAPI;
    obj["alertsAPI"] = alertsAPI;
    obj["city"] = city;
    obj["state"] = state;
    std::string json = bj::serialize(obj);
    std::ofstream outfile(settings_file, std::ios::out);
    if (outfile.is_open()) {
      outfile << json;
      // outfile.close(); //This can be omitted since the destructor will handle
      // it.
    } else {
      std::cerr << "Error opening " << settings_file << " for writing."
                << std::endl;
    }
  }
  void loadSettings() {
    if (settingsFileExists()) {
      std::ifstream ifile;
      ifile.open(settings_file);
      std::stringstream buffer;
      buffer << ifile.rdbuf();
      std::string json = buffer.str();
      ifile.close();

      bj::value jv = bj::parse(json);
      bj::object obj = jv.as_object();
      zipCode = obj.at("zipCode").as_string();
      delay = static_cast<int>(obj.at("delay").as_int64());
      retry = static_cast<int>(obj.at("retry").as_int64());
      periods = static_cast<int>(obj.at("periods").as_int64());
      forecastAPI = static_cast<std::string>(obj.at("forecastAPI").as_string());
      alertsAPI = static_cast<std::string>(obj.at("alertsAPI").as_string());
      city = static_cast<std::string>(obj.at("city").as_string());
      state = static_cast<std::string>(obj.at("state").as_string());
    } else {
      // std::cout << "settings.json does not exist!n";
    }
  }
  fs::path getExecuatablePath() {
#ifdef __linux__
    return fs::read_symlink("/proc/self/exe");
#elif _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return fs::path(buffer);
#else
    // Fallback to current_path if the platform isn't recognized.
    return fs::current_path();
#endif
  }
  bool settingsFileExists() {
    if (fs::exists(settings_file)) {
      return true;
    }
    return false;
  }

 private:
  fs::path settings_file;
  std::string zipCode{};
  int delay{};
  int retry{};
  int periods{};
  std::string city{};
  std::string state{};
  std::string forecastAPI{};
  std::string alertsAPI{};
};