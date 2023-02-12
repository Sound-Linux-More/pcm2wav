#include <stdio.h>
#include <stdlib.h>

void usage(char *command)
{
    printf("usage:\n"
           "\t%s wavfile pcmfile\n", command);
}

int main(int argc, char *argv[])
{
    FILE *pcmfile, *wavfile;
    unsigned long int wavfile_size;
    int read_len;
    unsigned char buf[1024];

    if (argc < 3)
    {
        usage(argv[0]);
        return 1;
    }

    wavfile = fopen(argv[1], "rb");
    if (wavfile == NULL)
    {
        printf("!Error: Can't open wavfile.\n");
        return 1;
    }
    fseek(wavfile, 0, SEEK_END);
    wavfile_size = ftell(wavfile);
    if (wavfile_size < 45)
    {
        printf("!Error: Not data in wavfile.\n");
        return 1;
    }
    fseek(wavfile, 44, SEEK_SET);

    pcmfile = fopen(argv[2], "wb");
    if (pcmfile == NULL)
    {
        printf("!Error: Can't create pcmfile.\n");
        return 1;
    }

    while((read_len = fread(buf, 1, sizeof(buf), wavfile)) != 0)
    {
        if(read_len != fwrite(buf, 1, read_len, pcmfile))
        {
            fprintf(stderr, "!Error: Can't write pcmfile.\n");
            return 1;
        }
        fflush(pcmfile);
    }

    fclose(pcmfile);
    fclose(wavfile);
}
