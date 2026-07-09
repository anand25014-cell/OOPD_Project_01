#ifndef PROTOCOL_H
#define PROTOCOL_H

// Protocol configuration struct for each technology
struct Protocol {
    const char* name;

    // Radio parameters
    int bandwidth_mhz;       // total bandwidth in MHz
    int channel_width_khz;   // channel width in kHz
    int num_channels;        // number of logical channels (derived or set)
    int users_per_channel;   // base users per channel (per antenna)
    int mimo;                // number of antennas / spatial layers

    // Logical derived parameters
    int messages_per_user;   // messages generated per user

    // convenience fields (computed)
    int channel_capacity;    // users_per_channel * mimo
};

// Instances
extern const Protocol Protocol2G;
extern const Protocol Protocol3G;
extern const Protocol Protocol4G;
extern const Protocol Protocol5G;

#endif
