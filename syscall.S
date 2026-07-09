#include "UserDevice.h"
#include "basicIO.h"

UserDevice::UserDevice(int _id, const Protocol* p, int req_ch)
    : id(_id), proto(p), messages_per_connection(p->messages_per_user),
      requested_channel(req_ch), assigned_channel_index(-1), assigned_antenna_index(-1)
{
#ifdef DEBUG
    io.outputstring("[DEBUG] UserDevice: created ID="); io.outputint(id);
    io.outputstring(" proto="); io.outputstring(proto->name);
    io.outputstring(" req_ch="); io.outputint(requested_channel);
    io.terminate();
#endif
}
