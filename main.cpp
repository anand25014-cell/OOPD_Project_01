#include "Protocol.h"

// ===========================
// 2G — Correct Data (PDF)
// ===========================
// - 1 MHz total spectrum
// - 200 kHz channels → 5 channels
// - 16 users per channel (TDMA)
// - 5 data msgs + 15 voice msgs = 20 msgs/user
// - No MIMO
const Protocol Protocol2G = {
    "2G",
    1,          // total bandwidth in MHz
    200,        // 200 kHz channel width
    5,          // 1 MHz / 200 kHz = 5 channels
    16,         // 16 users per channel
    1,          // no MIMO in 2G
    20,         // 5 data + 15 voice = 20 msgs
    16 * 1      // effective channel capacity
};

// ===========================
// 3G — Correct Data (PDF)
// ===========================
// - Same 1 MHz assumption
// - 200 kHz channels → 5 channels
// - 32 users per channel (CDMA)
// - 10 messages per user
// - No MIMO
const Protocol Protocol3G = {
    "3G",
    1,          // 1 MHz total bandwidth
    200,        // 200 kHz channels
    5,          // 5 channels
    32,         // 32 users per channel
    1,          // no MIMO
    10,         // messages per user
    32 * 1
};

// ===========================
// 4G — Correct Data (PDF)
// ===========================
// - OFDM splits into 10 kHz subchannels
// - 1 MHz spectrum → 100 subchannels
// - 30 users per 10 kHz
// - Supports MIMO with 4 antennas
// - 10 messages per user
const Protocol Protocol4G = {
    "4G",
    1,          // 1 MHz total
    10,         // 10 kHz sub-channel width
    100,        // 1 MHz / 10 kHz = 100 channels
    30,         // 30 users per subchannel
    4,          // 4-antennas MIMO
    10,         // 10 msgs per user
    30 * 4      // capacity per channel with MIMO
};

// ===========================
// 5G — Correct Data (PDF)
// ===========================
// - 10 MHz additional band at 1800 MHz
// - 30 users per 1 MHz
// - Total = 10 MHz → 10 channels
// - Massive MIMO: up to 16 antennas
// - 10 messages per user
const Protocol Protocol5G = {
    "5G",
    10,         // 10 MHz total spectrum
    1000,       // 1 MHz = 1000 kHz channel width
    10,         // 10 MHz / 1 MHz = 10 channels
    30,         // 30 users per MHz
    16,         // 16-antenna massive MIMO
    10,         // messages per user
    30 * 16     // per-channel capacity with 16 MIMO layers
};

