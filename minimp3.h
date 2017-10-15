#ifndef __MINIMP3_H_INCLUDED__
#define __MINIMP3_H_INCLUDED__

#define MP3_MAX_SAMPLES_PER_FRAME (1152*2)

struct _mp3_info
{
    int sample_rate;
    int channels;
    int audio_bytes;  // generated amount of audio per frame
};
typedef struct _mp3_info mp3_info_t;

typedef void* mp3_decoder_t;

mp3_decoder_t mp3_create(void);

/*
  Make sure to point to a buffer of at least MP3_MAX_SAMPLES_PER_FRAME
  samples (not bytes). The actual number of bytes (not samples) decoded
  is returned in the mp3_info_t structure field audio_bytes.
*/
int mp3_decode(mp3_decoder_t dec,
               void *buf,
               int bytes,
               signed short *out,
               mp3_info_t *info);

void mp3_done(mp3_decoder_t dec);

#define mp3_free(dec) do { mp3_done(dec); dec = NULL; } while(0)

//  https://www.mail-archive.com/busybox@busybox.net/msg12341.html

#endif//__MINIMP3_H_INCLUDED__
