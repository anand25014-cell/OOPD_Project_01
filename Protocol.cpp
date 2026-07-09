#include "CellTower.h"
#include "basicIO.h"

CellTower::CellTower(int tid, const Protocol* p, CellularCore* c)
    : id(tid), proto(p), core(c)
{
    // compute num_channels if not provided
    if (proto->num_channels > 0) num_channels = proto->num_channels;
    else {
        // derive from bandwidth (MHz) and channel width (kHz)
        int total_khz = proto->bandwidth_mhz * 1000;
        int width_khz = proto->channel_width_khz;
        if (width_khz <= 0) width_khz = 1;
        num_channels = total_khz / width_khz;
        if (num_channels <= 0) num_channels = 1;
    }
    mimo = proto->mimo;

    channels = GenericVector< GenericVector<UserDevice*> >(num_channels);
    for (int i = 0; i < num_channels; ++i) {
        channels[i] = GenericVector<UserDevice*>(0);
    }

    dropped_users = GenericVector<int>(0);
    dropped_users_channel = GenericVector<int>(0);

#ifdef DEBUG
    io.outputstring("[DEBUG] CellTower: created id="); io.outputint(id);
    io.outputstring(" proto="); io.outputstring(proto->name);
    io.outputstring(" channels="); io.outputint(num_channels);
    io.outputstring(" mimo="); io.outputint(mimo);
    io.terminate();
#endif
}

long CellTower::raw_capacity() const {
    long per_ch = proto->users_per_channel;
    return (long)num_channels * per_ch * (long)mimo;
}

bool CellTower::assign_user(UserDevice* u) {
    std::lock_guard<std::mutex> lk(tower_mutex);

#ifdef DEBUG
    io.outputstring("[DEBUG] assign_user: ID="); io.outputint(u->id);
    io.outputstring(" req_ch="); io.outputint(u->requested_channel);
    io.terminate();
#endif

    int ch = u->requested_channel;
    if (ch < 0 || ch >= num_channels) {
        dropped_users.push_back(u->id);
        dropped_users_channel.push_back(ch);
#ifdef DEBUG
        io.outputstring("[DEBUG] assign_user: dropped invalid ch ID="); io.outputint(u->id);
        io.outputstring(" ch="); io.outputint(ch); io.terminate();
#endif
        return false;
    }

    int capacity_per_channel = proto->users_per_channel * proto->mimo;
    if ((int)channels[ch].size() < capacity_per_channel) {
        channels[ch].push_back(u);
        u->assigned_channel_index = ch;
        u->assigned_antenna_index = (int)((channels[ch].size() - 1) % proto->mimo);
#ifdef DEBUG
        io.outputstring("[DEBUG] assign_user: assigned ID="); io.outputint(u->id);
        io.outputstring(" ch="); io.outputint(ch);
        io.outputstring(" ant="); io.outputint(u->assigned_antenna_index);
        io.terminate();
#endif
        return true;
    } else {
        dropped_users.push_back(u->id);
        dropped_users_channel.push_back(ch);
#ifdef DEBUG
        io.outputstring("[DEBUG] assign_user: dropped channel full ID="); io.outputint(u->id);
        io.outputstring(" ch="); io.outputint(ch); io.terminate();
#endif
        return false;
    }
}

long CellTower::total_messages() const {
    std::lock_guard<std::mutex> lk(tower_mutex);
    long total = 0;
    for (int ch = 0; ch < num_channels; ++ch) {
        for (unsigned int i = 0; i < channels[ch].size(); ++i) {
            total += channels[ch][i]->messages_per_connection;
        }
    }
    return total;
}

void CellTower::send_batch_to_core() {
    long msgs = total_messages();
#ifdef DEBUG
    io.outputstring("[DEBUG] send_batch_to_core: tower="); io.outputint(id);
    io.outputstring(" msgs="); io.outputint((int)msgs); io.terminate();
#endif
    core->enqueue_batch(MessageBatch(id, (int)msgs));
}

void CellTower::list_first_channel_users() const {
    std::lock_guard<std::mutex> lk(tower_mutex);
    io.outputstring("First channel users: ");
    if (num_channels <= 0) { io.outputstring("(no channels)"); io.terminate(); return; }
    const GenericVector<UserDevice*>& ch = channels[0];
    if (ch.size() == 0) { io.outputstring("(none)"); io.terminate(); return; }
    for (unsigned int i = 0; i < ch.size(); ++i) {
        io.outputint(ch[i]->id);
        if (i + 1 < ch.size()) io.outputstring(" ");
    }
    io.terminate();
}

void CellTower::print_summary() const {
    io.outputstring("=== CellTower Summary ==="); io.terminate();
    io.outputstring("Protocol: "); io.outputstring(proto->name); io.terminate();
    io.outputstring("Channels: "); io.outputint(num_channels); io.terminate();
    io.outputstring("Raw Capacity: "); io.outputint((int)raw_capacity()); io.terminate();

    long assigned = 0;
    for (int ch = 0; ch < num_channels; ++ch) assigned += channels[ch].size();
    io.outputstring("Assigned users total: "); io.outputint((int)assigned); io.terminate();

    for (int ch = 0; ch < num_channels; ++ch) {
        io.outputstring("Channel "); io.outputint(ch);
        io.outputstring(": Assigned="); io.outputint((int)channels[ch].size());
        int dropped_here = 0;
        for (unsigned int i = 0; i < dropped_users_channel.size(); ++i) {
            if (dropped_users_channel[i] == ch) dropped_here++;
        }
        io.outputstring(" Dropped="); io.outputint(dropped_here);
        io.terminate();
    }

    list_first_channel_users();

    io.outputstring("Dropped users total: "); io.outputint((int)dropped_users.size()); io.terminate();
    if (dropped_users.size() > 0) {
        io.outputstring("Dropped user IDs: ");
        for (unsigned int i = 0; i < dropped_users.size(); ++i) {
            io.outputint(dropped_users[i]);
            if (i + 1 < dropped_users.size()) io.outputstring(" ");
        }
        io.terminate();
    }
}
