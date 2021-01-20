#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "common/pimoroni_common.hpp"
#include <initializer_list>

namespace pimoroni {


  class PWMCluster {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static const uint64_t MAX_PWM_CLUSTER_WRAP = UINT16_MAX;  // UINT32_MAX works too, but seems to produce less accurate counters
    static const uint32_t LOADING_ZONE_SIZE = 3;              // The number of dummy transitions to insert into the data to delay the DMA interrupt (if zero then no zone is used)
    static const uint32_t LOADING_ZONE_POSITION = 55;         // The number of levels before the wrap level to insert the load zone
                                                              // Smaller values will make the DMA interrupt trigger closer to the time the data is needed,
                                                              // but risks stalling the PIO if the interrupt takes longer due to other processes
    static const bool DEFAULT_USE_LOADING_ZONE = true;        // Whether or not the default behaviour of PWMCluster is to use the loading zone
  public:
    static const uint BUFFER_SIZE = 64;     // Set to 64, the maximum number of single rises and falls for 32 channels within a looping time period
    static const uint NUM_BUFFERS = 3;


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct Transition {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
      uint32_t mask;
      uint32_t delay;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      Transition() : mask(0), delay(0) {};
    };

    struct Sequence {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
      uint32_t size;
      Transition data[BUFFER_SIZE];


      //--------------------------------------------------
      // Constructors/Destructor
      //-------------------------------