/**
 * @file CRTBernFrame.hpp
 *
 * Contains declaration of CRTBernFrame, a class for accessing/holding raw CRT data from the 'Bern' panels ProtoDUNE-II VD
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTBERNFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTBERNFRAME_HPP_

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

/**
 *  @brief Class for accessing/holding raw CRT data from the 'Bern' panels ProtoDUNE-II VD
 *
 */
    class CRTBernFrame
    {
    public:
        // ===============================================================
        // Preliminaries
        // ===============================================================

        // The definition of the format is in terms of 64-bit words
        typedef uint64_t word_t; // NOLINT

        static constexpr int      s_num_channels = 32;
        static constexpr uint64_t s_DTS_ticks_per_second = 62'500'000;
        static constexpr uint64_t s_ns_per_DTS_tick = 16;

        static constexpr int s_bits_per_adc = 16;
        static constexpr int s_bits_per_word = 8 * sizeof(word_t);
        static constexpr int s_num_adcs = 64;

        struct CRTBernData
        {
            uint16_t flags     = 0;
            uint16_t lostcpu   = 0;
            uint16_t lostfpga  = 0;
            uint32_t ts0       = 0;
            uint32_t ts1       = 0;
            uint16_t adc[32]   = {0};
            uint32_t coinc      = 0;
        };

        // ===============================================================
        // Data members
        // ===============================================================
        detdataformats::DAQEthHeader daq_header;
        uint16_t mac5;
        CRTBernData data;

        // ===============================================================
        // Accessors
        // ===============================================================

        /**
         * @brief Get the adc value for channel i_ch
         */
        uint16_t get_adc(int i_ch) const // NOLINT(build/unsigned)
        {
            if (i_ch < 0 || i_ch >= s_num_channels)
                throw std::out_of_range("ADC channel index out of range");

            return data.adc[i_ch];
        }

        /**
         * @brief Set the adc value for channel i_ch to @p val
         */
        void set_adc(int i_ch, uint16_t val) // NOLINT(build/unsigned)
        {
            if (i_ch < 0 || i_ch >= s_num_channels)
                throw std::out_of_range("ADC channel index out of range");

            data.adc[i_ch]=val;
        }

        /** @brief Get the starting 64-bit timestamp of the frame
         */
        uint64_t get_timestamp() const // NOLINT(build/unsigned)
        {
            return daq_header.get_timestamp() ; // NOLINT(build/unsigned)
        }

        /** @brief Set the starting 64-bit timestamp of the frame
         *  also set the underlying ts0 to be consistent
         */
        void set_timestamp(const uint64_t new_timestamp) // NOLINT(build/unsigned)
        {
            daq_header.timestamp = new_timestamp;
            data.ts0 = (new_timestamp % s_DTS_ticks_per_second) * s_ns_per_DTS_tick;
        }

        /** @brief Get the MAC5 identifier of the frame
         */
        uint16_t get_mac5() const // NOLINT(build/unsigned)
        {
            return mac5 ; // NOLINT(build/unsigned)
        }

        /** @brief Set the MAC5 identifier of the frame
         */
        void set_mac5(const uint16_t new_mac5) // NOLINT(build/unsigned)
        {
            mac5 = new_mac5;
        }

        /** @brief Get the flags field of the CRTBernData
        */
        uint16_t get_flags() const // NOLINT(build/unsigned)
        {
            return data.flags; // NOLINT(build/unsigned)
        }

        /** @brief Set the flags field of the CRTBernData
         */
        void set_flags(const uint16_t new_flags) // NOLINT(build/unsigned)
        {
            data.flags = new_flags;
        }

        /** @brief Get the lostcpu counter of the CRTBernData
         */
        uint16_t get_lostcpu() const // NOLINT(build/unsigned)
        {
            return data.lostcpu; // NOLINT(build/unsigned)
        }

        /** @brief Set the lostcpu counter of the CRTBernData
         */
        void set_lostcpu(const uint16_t new_lostcpu) // NOLINT(build/unsigned)
        {
            data.lostcpu = new_lostcpu;
        }

        /** @brief Get the lostfpga counter of the CRTBernData
        */
        uint16_t get_lostfpga() const // NOLINT(build/unsigned)
        {
            return data.lostfpga; // NOLINT(build/unsigned)
        }

        /** @brief Set the lostfpga counter of the CRTBernData
        */
        void set_lostfpga(const uint16_t new_lostfpga) // NOLINT(build/unsigned)
        {
            data.lostfpga = new_lostfpga;
        }

        /** @brief Get the ts0 timestamp of the CRTBernData
         */
        uint32_t get_ts0() const
        {
            return data.ts0;
        }

        /** @brief Set the ts0 timestamp of the CRTBernData
         */
        void set_ts0(const uint32_t new_ts0)
        {
            data.ts0 = new_ts0;
        }

        /** @brief Get the ts1 timestamp of the CRTBernData
        */
        uint32_t get_ts1() const
        {
            return data.ts1;
        }

        /** @brief Set the ts1 timestamp of the CRTBernData
        */
        void set_ts1(const uint32_t new_ts1)
        {
            data.ts1 = new_ts1;
        }


        /** @brief Get the coinc counter of the CRTBernData
        */
        uint32_t get_coinc() const
        {
            return data.coinc;
        }

        /** @brief Set the coinc counter of the CRTBernData
        */
        void set_coinc(const uint32_t new_coinc)
        {
            data.coinc = new_coinc;
        }


    }; //CRTBernFrame

} // namespace dunedaq::fddetdataformats


#endif //FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTBERNFRAME_HPP_
