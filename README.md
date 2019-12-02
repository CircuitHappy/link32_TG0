# Example project for Ableton Link on ESP32

## Timer Interrupt design for clock output at around 100 microseconds of accuracy.

Minimal example for using Ableton Link with esp-idf on ESP32.\
Tested with esp-idf release/v4.0.


Developed as part of the T-1 algorithmic sequencer by Torso Electronics.

More details on https://github.com/Ableton/link/pull/68.


## Usage
* Setup esp-idf following the instructions on https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html
* Clone repository with `git clone --recursive https://github.com/mathiasbredholt/link_esp32_example.git`
* Run `idf.py menuconfig`
   * Setup WiFi credentials under `Example Connection Configuration`
   * `Component Config > PThreads > Default Task Size` (4608)
   * `Component Config > LWIP > Max Number of Open Sockets` (16)
* Build and flash by running `idf.py build && idf.py flash`

