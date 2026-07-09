#include "CsvReader.h"
#include "basicIO.h"

// Helper: convert C string (null-terminated) to integer (like atoi)
// Handles optional leading spaces and optional leading '+' or '-'.
// Stops at first non-digit.
static int simple_atoi(const char* s) {
    if (!s) return 0;
    int i = 0;
    // skip spaces
    while (s[i] == ' ' || s[i] == '\t') ++i;
    int sign = 1;
    if (s[i] == '+') { ++i; }
    else if (s[i] == '-') { sign = -1; ++i; }
    int val = 0;
    while (s[i] >= '0' && s[i] <= '9') {
        val = val * 10 + (s[i] - '0');
        ++i;
    }
    return val * sign;
}

// Helper: remove trailing '\n' and '\r' from null-terminated buffer in-place
static void trim_newline(char* s) {
    if (!s) return;
    int i = 0;
    while (s[i] != '\0') ++i;
    // i is length
    while (i > 0) {
        char c = s[i-1];
        if (c == '\n' || c == '\r') { s[i-1] = '\0'; --i; }
        else break;
    }
}

GenericVector<CsvUserEntry> CsvReader::read_from_stdin() {
    GenericVector<CsvUserEntry> out;
    const int BUF_SZ = 1024;
    char buf[BUF_SZ];

    // Read header (discard)
    io.inputstring(buf, BUF_SZ);
#ifdef DEBUG
    io.outputstring("[DEBUG] CsvReader: header: "); io.outputstring(buf); io.terminate();
#endif
    if (buf[0] == '\0') return out;

    while (true) {
        io.inputstring(buf, BUF_SZ);
        if (buf[0] == '\0') break; // EOF or empty

        // trim newline/carriage returns
        trim_newline(buf);

        // find comma position manually
        int i = 0;
        int comma_index = -1;
        while (buf[i] != '\0') {
            if (buf[i] == ',') { comma_index = i; break; }
            ++i;
        }
        if (comma_index < 0) {
            // malformed line — skip
            continue;
        }

        // split: put null terminator at comma position
        buf[comma_index] = '\0';
        char* left = buf;
        char* right = buf + comma_index + 1;

        int uid = simple_atoi(left);
        int ch  = simple_atoi(right);

        CsvUserEntry e;
        e.user_id = uid;
        e.requested_channel = ch;
        out.push_back(e);

#ifdef DEBUG
        io.outputstring("[DEBUG] CsvReader: parsed -> ID=");
        io.outputint(uid); io.outputstring(" CH="); io.outputint(ch); io.terminate();
#endif
    }

    return out;
}
