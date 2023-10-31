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
#include "WeatherSettings.hpp"
namespace fs = std::filesystem;

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

std::string getValidZipCode(CommandLineProcessor& clp,
                            WeatherSettings& settings) {
  std::string zipCode = clp.getZipCode();
  std::regex zipCodeTest("^\\d{5,5}$");

  if (zipCode == "00000" && settings.settingsFileExists()) {
    zipCode = settings.getZipCode();
  }
  if (zipCode == "00000" || !std::regex_search(zipCode, zipCodeTest)) {
    do {
      std::cout << "Enter ZIP code: ";
      std::cin >> zipCode;
    } while (zipCode == "00000" || !std::regex_search(zipCode, zipCodeTest));
  }

  if (!std::regex_search(zipCode, zipCodeTest)) {
    throw std::invalid_argument("Invalid ZIP code.");
  }

  return zipCode;
}

void displayConfiguration(CommandLineProcessor& clp,
                          WeatherSettings& settings) {
  std::stringstream oss;
  int delay{}, retry{};
  if (clp.getDelay() != clp.getDefaultRefreshDelay())
    delay = clp.getDelay();
  else if (settings.getDelay() != clp.getDefaultRefreshDelay())
    delay = settings.getDelay();

  if (delay) oss << "Refresh delay set to " << delay << " minutes. ";

  if (clp.getRetry() != clp.getDefaultRetryDelay())
    retry = clp.getRetry();
  else if (settings.getRetry() != clp.getDefaultRetryDelay())
    retry = settings.getRetry();

  if (retry) oss << "Error retry delay set to " << retry << " minutes.";

  std::string outstring = oss.str();
  if (outstring.length()) std::cout << outstring << "\n";
}

void initializeVariables(std::string& zipCode, int& delay, int& retry,
                         int& forecastPeriods, std::string& forecast_api,
                         std::string& alerts_api, std::string& city,
                         std::string& state, WeatherSettings& settings) {
  zipCode = "00000";
  delay = 0;
  retry = 0;
  forecastPeriods = 0;
  forecast_api = "";
  alerts_api = "";
  city = "";
  state = "";
  settings.loadSettings();
}

void processCommandLineArgs(int ac, char* av[],
                            std::optional<CommandLineProcessor>& clp,
                            WeatherSettings& settings, std::string& zipCode,
                            int& delay, int& retry, int& forecastPeriods,
                            std::string& forecast_api, std::string& alerts_api,
                            std::string& city, std::string& state) {
  clp.emplace(ac, av);
  delay = clp->getDefaultRefreshDelay();
  retry = clp->getDefaultRetryDelay();
  forecastPeriods = clp->getDefaultForecastPeriods();

  if (clp->hasHelp()) {
    std::cout << clp->helpMessage() << '\n';
    exit(1);
  }

  zipCode = getValidZipCode(*clp, settings);
  if (settings.settingsFileExists()) {
    delay = settings.getDelay();
    retry = settings.getRetry();
    forecastPeriods = settings.getPeriods();
    // if the zipCode is the same as settings.json then the data is correct.
    // if forecast_api is left blank, new location will be retrieved.
    if (zipCode == settings.getZipCode()) {
      forecast_api = settings.getForecastAPI();
      alerts_api = settings.getAlertsAPI();
      city = settings.getCity();
      state = settings.getState();
    }
  }

  displayConfiguration(*clp, settings);
  if (clp->hasDelay()) delay = clp->getDelay();
  if (clp->hasRetry()) retry = clp->getRetry();
  if (clp->hasForecastPeriods()) forecastPeriods = clp->getForecastPeriods();

  settings.setZipCode(zipCode);
  settings.setDelay(delay);
  settings.setRetry(retry);
  settings.setForecastPeriods(forecastPeriods);
}

void setupWeatherLocation(const std::string& zipCode, std::string& forecast_api,
                          std::string& alerts_api, std::string& city,
                          std::string& state, WeatherSettings& settings) {
  if (forecast_api.empty()) {
    WeatherLocation myLocation(zipCode);
    forecast_api = myLocation.getForecastAPI();
    alerts_api = myLocation.getAlertsAPI();
    city = myLocation.getCity();
    state = myLocation.getState();
    settings.setForecastAPI(forecast_api);
    settings.setAlertsAPI(alerts_api);
    settings.setCity(city);
    settings.setState(state);
  }
  settings.saveSettings();
}

void displayWeatherLoop(WeatherSettings& settings,
                        const std::string& forecast_api,
                        const std::string& alerts_api, bool wordWrap) {
  HttpClient httpClient;
  while (true) {
    try {
      std::cout << "Run: \t\t" << getCurrentTimeStamp() << "\n";

      std::string rawData = httpClient.get(forecast_api);
      std::string rawAlerts = httpClient.get(alerts_api);

      WeatherData weatherData(rawData, rawAlerts, wordWrap);
      std::cout << weatherData.getGeneratedTime() << "\n";
      std::cout << weatherData.getUpdateTime() << "\n\n";
      weatherData.printAlerts();

      // Display the forecast for the next x periods
      const int NUM_FORECAST_PERIODS = settings.getPeriods();
      for (int i = 0; i < NUM_FORECAST_PERIODS; i++) {
        std::cout << weatherData.getForecastForPeriod(i);
        if (i != NUM_FORECAST_PERIODS - 1) std::cout << "\n";
      }

      std::cout << "---\n";
      std::this_thread::sleep_for(std::chrono::minutes(settings.getDelay()));

    } catch (const std::exception& e) {
      if (static_cast<std::string>(e.what()).substr(0, 12) == "out of range") {
        std::cerr << "weather.gov API unavailable. ";
      } else {
        std::cerr << "Error: " << e.what() << "\n";
      }

      const int RETRY_DELAY_MINUTES = settings.getRetry();
      std::cerr << "Retrying in " << RETRY_DELAY_MINUTES << " minutes. . .\n";
      std::this_thread::sleep_for(std::chrono::minutes(
          RETRY_DELAY_MINUTES));  // Wait for x minutes before retrying on
                                  // error
    }
  }
}

int main(int ac, char* av[]) {
  try {
    std::string zipCode, forecast_api, alerts_api, city, state;
    int delay, retry, forecastPeriods;
    std::optional<CommandLineProcessor> clp;
    WeatherSettings settings;

    initializeVariables(zipCode, delay, retry, forecastPeriods, forecast_api,
                        alerts_api, city, state, settings);

    processCommandLineArgs(ac, av, clp, settings, zipCode, delay, retry,
                           forecastPeriods, forecast_api, alerts_api, city,
                           state);

    setupWeatherLocation(zipCode, forecast_api, alerts_api, city, state,
                         settings);

    std::cout << "Weather for: \t" << city << ", " << state << '\n';

    displayWeatherLoop(settings, forecast_api, alerts_api, clp->getWordWrap());

  } catch (const std::exception& e) {
    std::cerr << "Unhandled exception: " << e.what() << '\n';
    return 1;
  } catch (const std::string& e) {
    std::cerr << "Unhandled exception: " << e << '\n';
    return 1;
  } catch (...) {
    std::cerr << "Unhandled exception!\n";
  }
  return 0;
}