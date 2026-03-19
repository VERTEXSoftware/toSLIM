#ifndef SLIM_STREAM_H
#define SLIM_STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <cstdio>

#ifndef SLIM_STREAM_MALLOC
#include <stdlib.h>
#define SLIM_STREAM_MALLOC(sz)  malloc(sz)
#define SLIM_STREAM_FREE(p)     free(p)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SLIM_STREAM SLIM_STREAM;

typedef enum {
    STREAM_MODE_READ         = 0,
    STREAM_MODE_WRITE        = 1,
    STREAM_MODE_APPEND       = 2,
    STREAM_MODE_READ_UPDATE  = 3,
    STREAM_MODE_WRITE_UPDATE = 4
} SLIM_STREAM_MODE;

typedef enum {
    SLIM_STREAM_SEEK_SET = 0,
    SLIM_STREAM_SEEK_CUR = 1,
    SLIM_STREAM_SEEK_END = 2
} SLIM_STREAM_SEEK_MODE;

typedef struct
{
    size_t (*read)(void* stream, void* data, size_t size, size_t count);
    size_t (*write)(void* stream, const void* data, size_t size, size_t count);
    int    (*seek)(void* stream, long long offset, SLIM_STREAM_SEEK_MODE origin);
    long long (*tell)(void* stream);
    long long (*size)(void* stream);
    void   (*close)(void* stream);
} SLIM_STREAM_CALLBACKS;

struct SLIM_STREAM
{
    void* streamdata;
    SLIM_STREAM_CALLBACKS cb;
    SLIM_STREAM_MODE mode;
};



SLIM_STREAM* SLIM_STREAM_OPEN(const char* filename, SLIM_STREAM_MODE mode);

SLIM_STREAM* SLIM_STREAM_CALLBACK(void* streamdata,const SLIM_STREAM_CALLBACKS* cb,SLIM_STREAM_MODE mode);


bool SLIM_STREAM_CLOSE(SLIM_STREAM* s);
bool SLIM_STREAM_ISOPEN(const SLIM_STREAM* s);

bool SLIM_STREAM_WRITE(SLIM_STREAM* s, const void* data, size_t size, size_t count);
bool SLIM_STREAM_READ(SLIM_STREAM* s, void* data, size_t size, size_t count);

bool SLIM_STREAM_SEEK(SLIM_STREAM* s, long long offset, SLIM_STREAM_SEEK_MODE origin);

long long SLIM_STREAM_TELL(const SLIM_STREAM* s);
long long SLIM_STREAM_SIZE(SLIM_STREAM* s);

#ifdef __cplusplus
}
#endif

#ifdef SLIM_STREAM_IMP

static size_t FSLIM_READ(void* stream, void* data, size_t size, size_t count)
{
    return fread(data, size, count, (FILE*)stream);
}

static size_t FSLIM_WRITE(void* stream, const void* data, size_t size, size_t count)
{
    return fwrite(data, size, count, (FILE*)stream);
}

static int FSLIM_SEEK(void* stream, long long offset, SLIM_STREAM_SEEK_MODE origin)
{
    int mode = SEEK_SET;

    switch (origin)
    {
    case SLIM_STREAM_SEEK_MODE::SLIM_STREAM_SEEK_SET:
        mode = SEEK_SET;
        break;
    case SLIM_STREAM_SEEK_MODE::SLIM_STREAM_SEEK_CUR:
        mode = SEEK_CUR;
        break;
    case SLIM_STREAM_SEEK_MODE::SLIM_STREAM_SEEK_END:
        mode = SEEK_END;
        break;
    default:
        break;
    }

    return fseek((FILE*)stream, (long)offset, mode);
}

static long long FSLIM_TELL(void* stream)
{
    return (long long)ftell((FILE*)stream);
}

static long long FSLIM_SIZE(void* stream)
{
    FILE* f = (FILE*)stream;

    long pos = ftell(f);

    if (pos < 0){return -1;}
    if (fseek(f, 0, SEEK_END) != 0){return -1;}

    long size = ftell(f);

    fseek(f, pos, SEEK_SET);

    return size;
}

static void FSLIM_CLOSE(void* stream)
{
    fclose((FILE*)stream);
}


SLIM_STREAM* SLIM_STREAM_OPEN(const char* filename, SLIM_STREAM_MODE mode)
{
    static const char* const modes[] = {"rb","wb","ab","rb+","wb+"};

    if ((unsigned int)mode >= 5){return NULL;}

    FILE* f = fopen(filename, modes[mode]);
    if (!f){ return NULL;}
    
    static const SLIM_STREAM_CALLBACKS file_callbacks =
    {
        FSLIM_READ,
        FSLIM_WRITE,
        FSLIM_SEEK,
        FSLIM_TELL,
        FSLIM_SIZE,
        FSLIM_CLOSE
    };

    return SLIM_STREAM_CALLBACK(f,&file_callbacks,mode);
}


SLIM_STREAM* SLIM_STREAM_CALLBACK(void* streamdata, const SLIM_STREAM_CALLBACKS* cb, SLIM_STREAM_MODE mode)
{
    if (!cb){ return NULL;}

    if ((unsigned int)mode >= 5){return NULL;}

    SLIM_STREAM* s = (SLIM_STREAM*)SLIM_STREAM_MALLOC(sizeof(SLIM_STREAM));

    if (!s){return NULL;}

    s->streamdata = streamdata;
    s->cb = *cb;
    s->mode = mode;

    return s;
}

bool SLIM_STREAM_CLOSE(SLIM_STREAM* s)
{
    if (!s) {return false;}

    if (s->cb.close){s->cb.close(s->streamdata);}

    SLIM_STREAM_FREE(s);
    return true;
}

bool SLIM_STREAM_ISOPEN(const SLIM_STREAM* s)
{
    return s != NULL;
}

bool SLIM_STREAM_WRITE(SLIM_STREAM* s, const void* data, size_t size, size_t count)
{
    if (!s || !s->cb.write) {return false;}

    return s->cb.write(s->streamdata, data, size, count) == count;
}

bool SLIM_STREAM_READ(SLIM_STREAM* s, void* data, size_t size, size_t count)
{
    if (!s || !s->cb.read) {return false;}

    return s->cb.read(s->streamdata, data, size, count) == count;
}

bool SLIM_STREAM_SEEK(SLIM_STREAM* s, long long offset, SLIM_STREAM_SEEK_MODE origin)
{
    if (!s || !s->cb.seek) {return false;}

    return s->cb.seek(s->streamdata, offset, origin) == 0;
}

long long SLIM_STREAM_TELL(const SLIM_STREAM* s)
{
    if (!s || !s->cb.tell){ return -1;}

    return s->cb.tell(s->streamdata);
}

long long SLIM_STREAM_SIZE(SLIM_STREAM* s)
{
    if (!s || !s->cb.size){ return -1;}

    return s->cb.size(s->streamdata);
}

#endif // SLIM_STREAM_IMP
#endif // SLIM_STREAM_H