#ifndef USER_DEVICE_H
#define USER_DEVICE_H

#include "Protocol.h"

class UserDevice {
public:
    int id; // unique user identifier
    const Protocol* proto; // which protocol this user uses
    int messages_per_connection; // number of messages this user will generate

    int requested_channel; // channel the user wants (from CSV)
    int assigned_channel_index; // actual channel assigned (-1 if none)
    int assigned_antenna_index;  // MIMO antenna index assigned (-1 if none)

    // parameterized constructor
    UserDevice(int _id, const Protocol* p, int req_ch);
};

#endif
