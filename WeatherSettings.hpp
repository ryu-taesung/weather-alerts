#pragma once

#include <boost/json/src.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
namespace bj = boost::json;

class WeatherSettings {
 public:
  WeatherSettings(std::string zipCode, int delay, int retry, int periods)
      : zipCode(zipCode), delay(delay), retry(retry), periods(periods) {}
  WeatherSettings() {}
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
    const fs::path settings{"settings.json"};
    std::ofstream outfile;
    outfile.open(settings.c_str());
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
    outfile << json;
    outfile.close();
  }
  void loadSettings() {
    const fs::path settings{"settings.json"};
    if (fs::exists(settings)) {
      std::ifstream ifile;
      ifile.open(settings.c_str());
      std::string json;
      ifile >> json;
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

 private:
  std::string zipCode{};
  int delay{};
  int retry{};
  int periods{};
  std::string city{};
  std::string state{};
  std::string forecastAPI{};
  std::string alertsAPI{};
  
};