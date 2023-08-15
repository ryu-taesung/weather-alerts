#include <iostream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <thread>

#include <curl/curl.h>

#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/json/src.hpp>

namespace bj = boost::json;

class HttpClient {
 private:
  static size_t writeFunction(void* ptr, size_t size, size_t nmemb,
                              std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
  }

 public:
  HttpClient() { curl_global_init(CURL_GLOBAL_DEFAULT); }

  ~HttpClient() { curl_global_cleanup(); }

  std::string get(const std::string& url) {
    auto curl = curl_easy_init();
    if (!curl) {
      throw std::runtime_error("Failed to initialize cURL");
    }

    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
                     "rts C++ client with libcurl/7.74.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    auto ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
      curl_easy_cleanup(curl);
      throw std::runtime_error(curl_easy_strerror(ret));
    }

    curl_easy_cleanup(curl);
    return response_string;
  }
};

class WeatherData {
 private:
  bj::value parsed_data;
  bj::value alert_data;

  boost::posix_time::ptime convertToLocalTime(const std::string& utc_time) {
    boost::posix_time::ptime time =
        boost::posix_time::from_iso_extended_string(utc_time.substr(0, 19));
    typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime>
        local_adj;
    return local_adj::utc_to_local(time);
  }

 public:
  WeatherData(const std::string& raw_data, const std::string& raw_alert) {
    parsed_data = bj::parse(raw_data);
    alert_data = bj::parse(raw_alert);
  }

  std::string getForecastForPeriod(int period) {
      bj::object properties =
          parsed_data.as_object().at("properties").as_object();
      bj::object forecast =
          properties.at("periods").as_array().at(period).as_object();
      return std::string(forecast.at("name").as_string().c_str()) + ": " +
             std::string(forecast.at("detailedForecast").as_string().c_str());
  }

  std::string getUpdateTime() {
      bj::object properties =
          parsed_data.as_object().at("properties").as_object();
      return "Updated: \t" +
             std::string(boost::posix_time::to_simple_string(convertToLocalTime(
                 properties.at("updated").as_string().c_str())));
  }

  std::string getGeneratedTime() {
      bj::object properties =
          parsed_data.as_object().at("properties").as_object();
      return "Generated: \t" +
             std::string(boost::posix_time::to_simple_string(convertToLocalTime(
                 properties.at("generatedAt").as_string().c_str())));
  }

  void printAlerts() {
      bj::array features = alert_data.as_object().at("features").as_array();
      for (auto& alert : features) {
        std::cout << "*** ";
        std::cout << alert.at("properties")
                         .as_object()
                         .at("event")
                         .as_string()
                         .c_str();
        std::cout << " - ";
        std::cout << alert.at("properties")
                         .as_object()
                         .at("headline")
                         .as_string()
                         .c_str();
        std::cout << "\n";
        std::cout << alert.at("properties")
                         .as_object()
                         .at("description")
                         .as_string()
                         .c_str();
        std::cout << "\n";
      }
      if (features.size()) std::cout << "\n";
  }
};

int main() {
  std::string forecast_api =
      "https://api.weather.gov/gridpoints/CTP/69,60/forecast";
  std::string alerts_api = "https://api.weather.gov/alerts/active/zone/PAC027";

  HttpClient httpClient;

  while (true) {
    try {
      // Get the current time point
      auto now = std::chrono::system_clock::now();
      // Convert to a time_t
      std::time_t current_time = std::chrono::system_clock::to_time_t(now);
      // Convert to local time
      std::tm* local_tm = std::localtime(&current_time);
      char timeBuffer[100];
      std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%b-%d %H:%M:%S",
                    local_tm);
      std::cout << "Run: \t\t" << timeBuffer << "\n";

      std::string rawData = httpClient.get(forecast_api);
      std::string rawAlerts = httpClient.get(alerts_api);

      WeatherData weatherData(rawData, rawAlerts);
      std::cout << weatherData.getGeneratedTime() << "\n";
      std::cout << weatherData.getUpdateTime() << "\n\n";
      weatherData.printAlerts();

      // Display the forecast for the next x periods
      const int NUM_FORECAST_PERIODS = 4;
      for (int i = 0; i < NUM_FORECAST_PERIODS; i++) {
        std::cout << weatherData.getForecastForPeriod(i) << "\n";
        if (i != NUM_FORECAST_PERIODS - 1) std::cout << "\n";
      }

      std::cout << "---\n";
      std::this_thread::sleep_for(std::chrono::minutes(90));

    } catch (const std::exception& e) {
      if (static_cast<std::string>(e.what()).substr(0,12) == "out of range"){
        std::cerr << "weather.gov API unavailable. ";
      } else {
        std::cerr << "Error: " << e.what() << "\n";
      }
      
      const int RETRY_DELAY_MINUTES = 5;
      std::cerr << "Retrying in " << RETRY_DELAY_MINUTES << " minutes. . .\n";
      std::this_thread::sleep_for(std::chrono::minutes(
          RETRY_DELAY_MINUTES));  // Wait for x minutes before retrying on error
    }
  }

  return 0;
}
