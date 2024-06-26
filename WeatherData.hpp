#pragma once

#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/json/src.hpp>

namespace bj = boost::json;

class WeatherData {
 private:
  bj::value parsed_data;
  bj::value alert_data;
  std::size_t width{80};
  static inline bool wordWrap = false;

  boost::posix_time::ptime convertToLocalTime(const std::string& utc_time) {
    boost::posix_time::ptime time =
        boost::posix_time::from_iso_extended_string(utc_time.substr(0, 19));
    typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime>
        local_adj;
    return local_adj::utc_to_local(time);
  }

 public:
  WeatherData(const std::string& raw_data, const std::string& raw_alert,
              const bool wrap) {
    parsed_data = bj::parse(raw_data);
    alert_data = bj::parse(raw_alert);
    wordWrap = wrap;
  }
  std::string textWrap(std::string input) {
    std::istringstream in{input};
    std::ostringstream out;
    std::string word, line, inputLine;

    while (std::getline(in, inputLine)) {
      std::istringstream lineStream(inputLine);

      while (lineStream >> word) {
        if (line.size() + 1 + word.size() >= width) {
          out << line << '\n';
          line = word;
        } else {
          if (!line.empty()) line += ' ';
          line += word;
        }
      }

      if (!line.empty()) {
        out << line << '\n';
        line.clear();
      }
    }
    return out.str();
  }

  std::string getForecastForPeriod(int period) {
    bj::object properties =
        parsed_data.as_object().at("properties").as_object();
    bj::object forecast =
        properties.at("periods").as_array().at(period).as_object();
    std::string output{
        std::string(forecast.at("name").as_string().c_str()) + ": " +
        std::string(forecast.at("detailedForecast").as_string().c_str())};
    if (wordWrap)
      return textWrap(output);
    else
      return output + "\n";
  }

  std::string getUpdateTime() {
    bj::object properties =
        parsed_data.as_object().at("properties").as_object();
    return "Updated: \t" +
           std::string(boost::posix_time::to_simple_string(convertToLocalTime(
               properties.at("updateTime").as_string().c_str())));
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
      std::cout
          << alert.at("properties").as_object().at("event").as_string().c_str();
      std::cout << " ***\n";
      std::cout << textWrap(alert.at("properties")
                                .as_object()
                                .at("headline")
                                .as_string()
                                .c_str());
      std::cout << "\n";
      std::cout << textWrap(alert.at("properties")
                                .as_object()
                                .at("description")
                                .as_string()
                                .c_str());
      std::cout << "\n";
    }
    if (features.size()) std::cout << "\n";
  }
};
