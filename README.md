# Weather Alerts

## Overview
Weather Alerts is a C++ application that uses libcurl and Boost libraries to provide current weather forecast (default 7 periods), and weather alert notifications. This application is easy to integrate and use in various C++ projects.

## Prerequisites
Before building Weather Alerts, ensure you have the following dependencies available:
- `libcurl-dev`
- `Boost program_options`
- `Boost json`

## Installation Instructions

### Step 1: Install libcurl
If `libcurl-dev` is not already installed on your system, you can install it using the following command:
`sudo apt install libcurl-dev`

(Vcpkg installation instructions will be provided soon.)

### Step 2: Install Boost Libraries
First, download the Boost library from [Boost Official Website](https://www.boost.org/).

Then, navigate to the Boost directory and run the following commands to install the required Boost components:
```
./bootstrap
./b2 --with-program_options runtime-link=static link=static address-model=64
```

### Step 3: Compile the Weather Alerts Application
Use the following command to compile the `weather-alerts` application:

```
g++ -Wall -std=c++17 -o weather-alerts -I../boost_1_83_0 weather-alerts.cpp -lcurl -lpthread -L../boost_1_83_0/stage/lib -lboost_program_options
```
Or run the Linux makefile:
`make`

## Usage
After successful installation, you can run the `weather-alerts` executable to receive weather alerts.

## Contributing
Contributions to the Weather Alerts project are welcome. Please feel free to fork the repository, make changes, and submit a pull request.

## License
[Specify the license here, if applicable]

## Contact
For any queries or contributions, please contact [Your Contact Information].

---

Weather Alerts - An efficient way to stay updated with the weather.
