#include <stdio.h>
#include <string.h>
#include <stdint.h>

#if 1
#define swap_u16
#define swap_16
#define swap_u32
#define swap_32
#define swap_u64
#define swap_64
#else
#define swap_u16 swap_uint16
#define swap_16 swap_int16
#define swap_u32 swap_uint32
#define swap_32 swap_int32
#define swap_u64 swap_uint64
#define swap_64 swap_int64
#endif

////////////// Endian swaping functions //////////

//! Byte swap unsigned short
uint16_t swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

//! Byte swap short
int16_t swap_int16( int16_t val )
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

//! Byte swap unsigned int
uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

//! Byte swap int
int32_t swap_int32( int32_t val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

//! Byte swap int64
int64_t swap_int64( int64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}

//! Byte swap uint64
uint64_t swap_uint64( uint64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | (val >> 32);
}

////////////// Endian swaping wrapper functions //////////
void fwrite64(int64_t value, FILE *f)
{
    int64_t item = swap_64(value);
    fwrite(&item, sizeof(int64_t),1,f);
}

void fwriteU64(uint64_t value, FILE *f)
{
    uint64_t item = swap_u64(value);
    fwrite(&item, sizeof(uint64_t),1,f);
}


void fwrite32(int32_t value, FILE *f)
{
    int32_t item = swap_32(value);
    fwrite(&item, sizeof(int32_t),1,f);
}

void fwriteU32(uint32_t value, FILE *f)
{
    uint32_t item = swap_u32(value);
    fwrite(&item, sizeof(uint32_t),1,f);
}

void fwrite16(int16_t value, FILE *f)
{
    int16_t item = swap_16(value);
    fwrite(&item, sizeof(int16_t),1,f);
}

void fwriteU16(uint16_t value, FILE *f)
{
    uint16_t item = swap_u16(value);
    fwrite(&item, sizeof(uint16_t),1,f);
}

void fwrite8(int8_t *value, FILE *f)
{
    fwrite(value, sizeof(int8_t), strlen(value),f);
}

void fwriteU8(uint8_t *value, FILE *f)
{
    fwrite(value, sizeof(uint8_t), strlen(value),f);
}

void usage(char *command)
{
    printf("usage:\n"
           "\t%s pcmfile wavfile channel samplerate bitspersample\n", command);
}

int main(int argc, char *argv[])
{
    FILE *pcmfile, *wavfile;
    unsigned long int pcmfile_size, chunk_size, total_size;
    int read_len;
    int chunkSize = 16;
    short audioFormat = 1;
    short NumChannels, BitsPerSample, BlockAlign;
    int SamplingRate, ByteRate;
    unsigned char buf[1024];

    if (argc != 6)
    {
        usage(argv[0]);
        return 1;
    }

    pcmfile = fopen(argv[1], "rb");
    if (pcmfile == NULL)
    {
        fprintf(stderr, "!Error: Can't open pcmfile.\n");
        return 1;
    }
    fseek(pcmfile, 0, SEEK_END);
    pcmfile_size = ftell(pcmfile);
    fseek(pcmfile, 0, SEEK_SET);
    total_size = 36 + pcmfile_size;

    wavfile = fopen(argv[2], "wb");
    if (wavfile == NULL)
    {
        fprintf(stderr, "!Error: Can't create wavfile.\n");
        return 1;
    }
    NumChannels = atoi(argv[3]);
    SamplingRate = atoi(argv[4]);
    BitsPerSample = atoi(argv[5]);
    ByteRate = NumChannels * BitsPerSample * SamplingRate / 8;
    BlockAlign = NumChannels * BitsPerSample / 8;

    fwrite8("RIFF", wavfile);
    fwrite32(total_size, wavfile);
    fwrite8("WAVE", wavfile);
    fwrite8("fmt ", wavfile);
    fwrite32(chunkSize, wavfile);
    fwrite16(audioFormat, wavfile);
    fwrite16(NumChannels, wavfile);
    fwrite32(SamplingRate, wavfile);
    fwrite32(ByteRate, wavfile);
    fwrite16(BlockAlign, wavfile);
    fwrite16(BitsPerSample, wavfile);
    fwrite8("data", wavfile);
    fwrite32(pcmfile_size, wavfile);
    fflush(wavfile);

    while((read_len = fread(buf, 1, sizeof(buf), pcmfile)) != 0)
    {
        if(read_len != fwrite(buf, 1, read_len, wavfile))
        {
            fprintf(stderr, "!Error: Can't write wavfile.\n");
            return 1;
        }
        fflush(wavfile);
    }

    fclose(pcmfile);
    fclose(wavfile);
}
