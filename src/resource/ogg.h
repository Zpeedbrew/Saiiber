#ifndef OGG_H
#define OGG_H
#include <gctypes.h>

struct OggFile {
    u8 *data;
    u32 size;
    u32 pos;
};


class Ogg {
public:
    Ogg(const char* filename);
    ~Ogg();

private:
    void* buffer;
    long size;
};

#endif // OGG_H