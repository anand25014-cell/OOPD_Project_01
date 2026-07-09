#ifndef CELL_TOWER_H
#define CELL_TOWER_H

#include "Protocol.h"
#include "UserDevice.h"
#include "CellularCore.h"
#include "GenericVector.h"
#include "basicIO.h"
#include <mutex>

class CellTower {
public:
    int id;
    const Protocol* proto;
    CellularCore* core;

    int num_channels;
    int mimo;

    // channels[ch] = vector of UserDevice*
    GenericVector< GenericVector<UserDevice*> > channels;

    // dropped users (user id) and requested channel (parallel arrays)
    GenericVector<int> dropped_users;
    GenericVector<int> dropped_users_channel;

    mutable std::mutex tower_mutex;

    CellTower(int tid, const Protocol* p, CellularCore* c);

    long raw_capacity() const;
    bool assign_user(UserDevice* u); // tries requested channel only
    long total_messages() const;

    void send_batch_to_core();
    void print_summary() const;

    void list_first_channel_users() const;
};

#endif
