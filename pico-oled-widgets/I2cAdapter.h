#pragma once

#include "hardware/i2c.h"

#include <cstdlib>
#include <functional>

template <i2c_inst_t* I2cInst>
class I2cAdapter
{
public:
    static void writeData(
        const uint8_t address,
        const uint8_t* const data,
        const size_t length
    ) {
        i2c_write_blocking(
            I2cInst,
            address,
            data,
            length,
            false
        );
    }

    static void writeData(
        const uint8_t address,
        const uint8_t prefixByte,
        const uint8_t* const data,
        const size_t length,
        const std::function<uint8_t (uint8_t)>& byteManipulator = [](const uint8_t b) { return b; }
    ) {
        auto i2cWaitForTransmit = [] {
            bool abort = false;
            do {
                abort = (bool) I2cInst->hw->clr_tx_abrt;
                tight_loop_contents();
            } while (!abort && !(I2cInst->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS));
            return !abort;
        };

        auto i2cWaitForStop = [] {
            do {
                tight_loop_contents();
            } while (!(I2cInst->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_STOP_DET_BITS));
        };

        I2cInst->hw->enable = 0;
        I2cInst->hw->tar = address;
        I2cInst->hw->enable = 1;

        I2cInst->hw->data_cmd = prefixByte;
        
        if (!i2cWaitForTransmit()) {
            return;
        }

        for (auto i = 0u; i < length; ++i) {
            bool lastByte = i == length - 1;

            I2cInst->hw->data_cmd =
                bool_to_bit(lastByte) << I2C_IC_DATA_CMD_STOP_LSB |
                byteManipulator(data[i]);

            if (!i2cWaitForTransmit()) {
                return;
            }

            if (lastByte) {
                i2cWaitForStop();
            }
        }
    }
};