#include <chrono>
#include <iostream>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <thread>

#include "CommandLineProcessor.hpp"
#include "HttpClient.hpp"
#include "WeatherData.hpp"
#include "WeatherLocation.hpp"

std::string getCurrentTimeStamp() {
  // Get the current time point
  auto now = std::chrono::system_clock::now();
  // Convert to a time_t
  std::time_t current_time = std::chrono::system_clock::to_time_t(now);
  // Convert to local time
  std::tm* local_tm = std::localtime(&current_time);
  char timeBuffer[32];
  std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%b-%d %H:%M:%S", local_tm);

  return static_cast<std::string>(timeBuffer);
}

std::string getValidZipCode(CommandLineProcessor& clp) {
  std::string zipCode = clp.getZipCode();
  std::regex zipCodeTest("^\\d{5,5}$");

  if (zipCode == "00000") {
    do {
      std::cout << "Enter ZIP code: ";
      std::cin >> zipCode;
    } while (zipCode == "00000" || !std::regex_search(zipCode, zipCodeTest));
  }

  // if (!std::regex_search(zipCode, zipCodeTest)) {
  //   throw std::invalid_argument("Invalid ZIP code.");
  // }

  return zipCode;
}

void displayConfiguration(CommandLineProcessor& clp) {
  std::stringstream oss;
  if (clp.getDelay() != clp.getDefaultRefreshDelay()) {
    oss << "Refresh delay set to " << clp.getDelay() << " minutes. ";
  }
  if (clp.getRetry() != clp.getDefaultRetryDelay()) {
    oss << "Error retry delay set to " << clp.getRetry() << " minutes.";
  }
  std::string outstring = oss.str();
  if (outstring.length()) std::cout << outstring << "\n";
}

int main(int ac, char* av[]) {
  std::string zipCode = "00000";
  std::optional<CommandLineProcessor> clp;
  try {
    try {
      clp.emplace(ac, av);

      if (clp->hasHelp()) {
        std::cout << clp->helpMessage() << 'n';
        return 1;
      }

      zipCode = getValidZipCode(*clp);
      displayConfiguration(*clp);

    } catch (std::exception& e) {
      std::cout << "Error: " << e.what() << '\n';
      return 1;
    }

    WeatherLocation myLocation(zipCode);
    std::string forecast_api = myLocation.getForecastAPI();
    std::string alerts_api = myLocation.getAlertsAPI();
    std::cout << "Weather for: \t" << myLocation.getCity() << ", "
              << myLocation.getState() << '\n';

    HttpClient httpClient;

    while (true) {
      try {
        std::cout << "Run: \t\t" << getCurrentTimeStamp() << "\n";

        std::string rawData = httpClient.get(forecast_api);
        std::string rawAlerts = httpClient.get(alerts_api);

        WeatherData weatherData(rawData, rawAlerts);
        std::cout << weatherData.getGeneratedTime() << "\n";
        std::cout << weatherData.getUpdateTime() << "\n\n";
        weatherData.printAlerts();

        // Display the forecast for the next x periods
        const int NUM_FORECAST_PERIODS = clp->getForecastPeriods();
        for (int i = 0; i < NUM_FORECAST_PERIODS; i++) {
          std::cout << weatherData.getForecastForPeriod(i) << "\n";
          if (i != NUM_FORECAST_PERIODS - 1) std::cout << "\n";
        }

        std::cout << "---\n";
        std::this_thread::sleep_for(std::chrono::minutes(clp->getDelay()));

      } catch (const std::exception& e) {
        if (static_cast<std::string>(e.what()).substr(0, 12) ==
            "out of range") {
          std::cerr << "weather.gov API unavailable. ";
        } else {
          std::cerr << "Error: " << e.what() << "\n";
        }

        const int RETRY_DELAY_MINUTES = clp->getRetry();
        std::cerr << "Retrying in " << RETRY_DELAY_MINUTES << " minutes. . .\n";
        std::this_thread::sleep_for(std::chrono::minutes(
            RETRY_DELAY_MINUTES));  // Wait for x minutes before retrying on
                                    // error
      }
    }
  } catch (std::exception& e) {
    std::cerr << "Unhandled exception: " << e.what() << '\n';
    return 1;
  } catch (std::string& e) {
    std::cerr << "Unhandled exception: " << e << '\n';
    return 1;
  } catch (...) {
    std::cerr << "Unhandled exception!\n";
  }

  return 0;
}
