/**
 * Copyright (c) 2017
 * Circuit Happy, LLC
 */

#include <iostream>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "gpio.hpp"

using std::string;
using namespace MissingLink;
using namespace MissingLink::GPIO;

Pin::Pin(gpio_num_t address, Direction direction)
  : m_address(address)
  , m_direction(direction)
{
  open();
}

Pin::~Pin() {
  close();
}

void Pin::Write(DigitalValue value) {
  if (m_direction == IN) {
    std::cerr << "Attempt to write to input pin at " + m_address << std::endl;
    return;
  }
  write(value);
}

DigitalValue Pin::Read() {
  if (m_direction == OUT) {
    std::cerr << "Attempt to read from output pin at " + m_address << std::endl;
    return DigitalValue::LOW;
  }
  return read();
}

void Pin::SetEdgeMode(Edge edge) {
  if (m_direction == OUT) {
    std::cerr << "Attempt to set edge mode for output pin at " + m_address << std::endl;
    return;
  }

  gpio_intr_enable(m_address);

  gpio_int_type_t edge_t;
  switch (edge) {
    case RISING:
      edge_t = GPIO_INTR_POSEDGE;
      break;
    case FALLING:
      edge_t = GPIO_INTR_NEGEDGE;
      break;
    case BOTH:
      edge_t = GPIO_INTR_ANYEDGE;
      break;
    default:
      return;
  }
  gpio_set_intr_type(m_address, edge_t);
}

void Pin::write(DigitalValue value) {
  gpio_set_level(m_address, value);
}

DigitalValue Pin::read() {
  int value = gpio_get_level(m_address);
  return (value == 1) ? LOW : HIGH;
}

void Pin::open() {
  esp_err_t result;

  gpio_config_t io_conf;

  uint32_t pin_num = 1ULL<<m_address;

  if (m_direction == IN) {
    //Input Mode
    //enable interrupt
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = pin_num;
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    //enable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t)1;
  } else {
    //Output Mode
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = pin_num;
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    //disable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t)0;
  }

  //configure GPIO with the given settings
  result = gpio_config(&io_conf);

  if (result == ESP_ERR_INVALID_ARG) {
    std::cerr << "Failed to config pin number: " << m_address << std::endl;
  }
}

void Pin::close() {
  //do we need to do anything here? I guess we could set things back to defaults
}
