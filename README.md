# Weather Alerts

**Weather Alerts** is a lightweight C++ application that fetches and displays current weather forecasts and alerts using libcurl and Boost.
It’s designed to be simple to build, portable, and easy to integrate into other projects.

---

## Quick Start (Recommended)

Most users can install the required dependencies via their system package manager and build immediately.

### Fedora / RHEL / Rocky / Alma

```bash
sudo dnf update
sudo dnf install boost-devel libcurl-devel gcc-c++ make
```

### Debian / Ubuntu

```bash
sudo apt update
sudo apt install libboost-all-dev libcurl4-openssl-dev build-essential
```

### Build

Once dependencies are installed:

```bash
make
```

That’s it — the `weather-alerts` binary will be produced in the project directory.

---

## Manual / Advanced Build (Optional)

If you prefer to build Boost manually (for custom versions, static linking, or non-standard environments), follow the steps below.

### 1. Install libcurl

**Debian / Ubuntu**

```bash
sudo apt install libcurl4-openssl-dev
```

**Fedora**

```bash
sudo dnf install libcurl-devel
```

---

### 2. Build Boost Manually

Download Boost from:
[https://www.boost.org/](https://www.boost.org/)

Then:

```bash
./bootstrap.sh
./b2 --with-program_options runtime-link=static link=static address-model=64
```

> Note: Adjust `address-model` or linkage options if targeting non-x86_64 systems.

---

### 3. Build Weather Alerts Manually

Example compile command:

```bash
g++ -Wall -std=c++17 \
  -I/path/to/boost \
  weather-alerts.cpp \
  -L/path/to/boost/stage/lib \
  -lboost_program_options \
  -lcurl \
  -lpthread \
  -o weather-alerts
```

Or simply use:

```bash
make
```

if you’ve configured the Makefile appropriately.

---

## Usage

```bash
./weather-alerts [options]
```

### Available Options

```
-h, --help                 Show help message
-z, --zipcode <zip>        US ZIP code for weather forecast
-p, --periods <n>          Number of forecast periods
-w, --wordwrap             Enable word wrapping (default: enabled)
-d, --delay <minutes>      Refresh interval
-r, --retry <minutes>      Retry delay on error
```

---

## Notes

* Designed to be lightweight and dependency-minimal.
* Works well on servers, SBCs, and embedded Linux systems.
* Boost is only used for argument parsing and can be replaced if desired.

---

## Contributing

Pull requests are welcome!
If you’re adding features or modifying build behavior, please document changes clearly.

---

## License

[Specify license here]

