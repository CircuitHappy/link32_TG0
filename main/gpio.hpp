/**
 * Copyright (c) 2017
 * Circuit Happy, LLC
 */

#pragma once

#include <string>
#include <memory>
#include <driver/gpio.h>

namespace MissingLink {
namespace GPIO {

enum DigitalValue {
  LOW = 0,
  HIGH = 1
};

class Pin {

  public:

    enum Direction {
      IN = 0,
      OUT = 1
    };

    enum Edge {
      RISING,
      FALLING,
      BOTH
    };

    Pin(gpio_num_t address, Direction direction);
    virtual ~Pin();

    DigitalValue Read();
    void Write(DigitalValue value);

    void SetEdgeMode(Edge edge);

  protected:

    const gpio_num_t m_address;
    const Direction m_direction;

    void open();
    void close();

    DigitalValue read();
    void write(DigitalValue value);

};

}} // namespaces
