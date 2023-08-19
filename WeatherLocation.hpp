#pragma once

#include <boost/json/src.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <regex>
#include <stdexcept>

namespace bj = boost::json;

#include "HttpClient.hpp"

class WeatherLocation {
 public:
  WeatherLocation(std::string zipCode) : zipCode(zipCode) {
    std::regex zipCodeTest("^\\d{5,5}");
    if (!std::regex_search(zipCode, zipCodeTest)) {
      throw std::runtime_error("Invalid ZIP code!n");
    }
    std::string zipXML = httpClient.get(zipXMLUrl + zipCode);
    latLong = setLatLong(zipXML);
    std::string gridJSON = httpClient.get(grid_api_base + latLong);
    try {
      parseJson(gridJSON);
    } catch (std::exception& e) {
      std::cout << "parseJSON Error: " << e.what() << 'n';
      throw std::string("parseJSON exception");
    }
  }
  std::string getCity() { return city; }

  std::string getState() { return state; }

  std::string getForecastAPI() { return forecast_api; }

  std::string getAlertsAPI() { return alerts_api; }

 private:
  std::string setLatLong(std::string zipXML) {
    std::string output;
    std::stringstream ss;
    try {
      ss << zipXML;
      boost::property_tree::ptree pt;
      read_xml(ss, pt);
      output = pt.get<std::string>("dwml.latLonList");
    } catch (std::exception& e) {
      std::cout << "Error determining lat/long from zip code.n";
      std::cout << "Error: " << e.what() << std::endl;
    }
    return output;
  }

  void parseJson(std::string gridJSON) {
    bj::value data = bj::parse(gridJSON);
    bj::object obj = data.as_object();
    city = obj.at("properties")
               .as_object()
               .at("relativeLocation")
               .as_object()
               .at("properties")
               .as_object()
               .at("city")
               .as_string()
               .c_str();
    state = obj.at("properties")
                .as_object()
                .at("relativeLocation")
                .as_object()
                .at("properties")
                .as_object()
                .at("state")
                .as_string()
                .c_str();
    forecast_api =
        obj.at("properties").as_object().at("forecast").as_string().c_str();
    std::string alerts_string =
        obj.at("properties").as_object().at("county").as_string().c_str();
    std::string alertsZone = alerts_string.substr(
        alerts_string.find_last_of("/") + 1, alerts_string.length());
    alerts_api = alerts_api_base + alertsZone;
  }

  HttpClient httpClient;
  std::string zipXMLUrl =
      "https://graphical.weather.gov/xml/sample_products/browser_interface/"
      "ndfdXMLclient.php?listZipCodeList=";
  std::string zipCode;
  std::string alerts_api_base = "https://api.weather.gov/alerts/active/zone/";
  std::string alerts_api;
  std::string grid_api_base = "https://api.weather.gov/points/";
  std::string grid_api;
  std::string forecast_api;
  std::string latLong;
  std::string city, state;
};
