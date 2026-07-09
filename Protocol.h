#ifndef CSV_READER_H
#define CSV_READER_H

#include "GenericVector.h"

struct CsvUserEntry {
    int user_id;
    int requested_channel;
};

class CsvReader {
public:
    static GenericVector<CsvUserEntry> read_from_stdin();
};

#endif
