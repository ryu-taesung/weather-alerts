#pragma once

#include <boost/program_options.hpp>

class CommandLineProcessor {
 private:
  boost::program_options::variables_map argv_vm;
  boost::program_options::options_description desc;
  static inline const int REFRESH_DELAY_MINUTES = 90;
  static inline const int RETRY_DELAY_MINUTES = 5;
  static inline const int FORECAST_PERIODS = 7;

 public:
  CommandLineProcessor(int ac, char* av[]) : desc("Allowed options") {
    desc.add_options()("help,h", "produce help message")(
        "zipcode,z",
        boost::program_options::value<std::string>()->default_value("00000"),
        "US Zip code for weather forecast")(
        "periods,p", boost::program_options::value<int>(),
        "# of forecast periods to display")(
        "wordwrap,w",
        boost::program_options::value<bool>()->default_value(true),
        "Word wrap output")("delay,d", boost::program_options::value<int>(),
                            "Refresh delay in minutes")(
        "retry,r",
        boost::program_options::value<int>()->default_value(
            RETRY_DELAY_MINUTES),
        "Error retry delay in minutes");

    boost::program_options::positional_options_description p;
    p.add("zipcode", -1);

    boost::program_options::store(
        boost::program_options::command_line_parser(ac, av)
            .options(desc)
            .allow_unregistered()
            .style(
                boost::program_options::command_line_style::unix_style |
                boost::program_options::command_line_style::allow_long_disguise)
            .positional(p)
            .run(),
        argv_vm);

    boost::program_options::notify(argv_vm);
  }

  bool hasHelp() const { return argv_vm.count("help"); }

  bool hasDelay() const { return argv_vm.count("delay"); }

  bool hasRetry() const { return argv_vm.count("retry"); }

  bool hasForecastPeriods() const { return argv_vm.count("periods"); }

  int getDelay() const {
    int delay{REFRESH_DELAY_MINUTES};
    if (argv_vm.count("delay")) {
      delay = argv_vm["delay"].as<int>();
    }
    return delay;
  }

  int getRetry() const { return argv_vm["retry"].as<int>(); }

  bool getWordWrap() const { return argv_vm["wordwrap"].as<bool>(); }

  int getDefaultRefreshDelay() const { return REFRESH_DELAY_MINUTES; }

  int getDefaultRetryDelay() const { return RETRY_DELAY_MINUTES; }

  int getForecastPeriods() const {
    int periods = FORECAST_PERIODS;
    if (argv_vm.count("periods")) {
      periods = argv_vm["periods"].as<int>();
    }
    return periods;
  }

  int getDefaultForecastPeriods() const { return FORECAST_PERIODS; }

  std::string getZipCode() const {
    return argv_vm["zipcode"].as<std::string>();
  }

  std::string helpMessage() const {
    std::ostringstream os;
    os << desc;
    return os.str();
  }
};
