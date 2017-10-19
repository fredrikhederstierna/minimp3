// minimp3 example player application for Linux/ALSA
// modified from OSS to ALSA by Fredrik Hederstierna 2017
// this file is public domain -- do with it whatever you want!
#include "libc.h"
#include "minimp3.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>

size_t strlen(const char *s);
#define out(text) do { int dummy = write(1, (const void *) text, strlen(text)); (void)dummy; } while(0)

#define WRITE_DECODED_FILE

int main(int argc, char *argv[])
{
    mp3_decoder_t mp3;
    mp3_info_t info;
    void *file_data;
    unsigned char *stream_pos;
    signed short sample_buf[MP3_MAX_SAMPLES_PER_FRAME];
    int bytes_left;
    int frame_size;

    out("minimp3 -- a small MPEG-1 Audio Layer III player based on ffmpeg\n\n");
    if (argc < 2) {
        out("Error: no input file specified!\n");
        return 1;
    }
    if (argc < 3) {
        out("Error: no sound dev specified!\n");
        return 1;
    }

    int fdr = open(argv[1], O_RDONLY);
    if (fdr < 0) {
        out("Error: cannot open `");
        out(argv[1]);
        out("'!\n");
        return 1;
    }

#ifdef WRITE_DECODED_FILE
    int fdw = open("decoded.pcm", O_WRONLY | O_CREAT, 0644);
    if (fdw < 0) {
        out("Error: cannot open write file.\n");
        return 1;
    }
#endif
    
    bytes_left = lseek(fdr, 0, SEEK_END);
    file_data = mmap(0, bytes_left, PROT_READ, MAP_PRIVATE, fdr, 0);
    stream_pos = (unsigned char *) file_data;
    bytes_left -= 100;
    int tot_mp3_size = bytes_left;

    out("Now Playing: ");
    out(argv[1]);

    mp3 = mp3_create();

    // decode first frame
    frame_size = mp3_decode(mp3,
                            stream_pos,
                            bytes_left,
                            sample_buf,
                            &info);
    if (!frame_size) {
        out("\nError: not a valid MP3 audio file!\n");
        return 1;
    }

    snd_pcm_t *playback_handle;
    snd_pcm_hw_params_t *hw_params;

    printf("snd_pcm_open\n");
    
    /* Open the device */
    snd_pcm_open (&playback_handle, argv[2], SND_PCM_STREAM_PLAYBACK, 0);

    printf("snd_pcm_hw_params_malloc\n");
    
    /* Allocate Hardware Parameters structures and fills it with config space for PCM */
    snd_pcm_hw_params_malloc (&hw_params);
    snd_pcm_hw_params_any (playback_handle, hw_params);

    printf("snd_pcm_hw_params_set\n");
    
    /* Set parameters : interleaved channels, 16 bits little endian, 8000Hz, 1 channel */
    snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_NONINTERLEAVED);
    snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    unsigned int val = 8000;
    int dir = 0;
    snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &val, &dir);
    // 1 channel mono
    snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1);

    printf("snd_pcm_hw_params2\n");
        
    /* Assign them to the playback handle and free the parameters structure */
    snd_pcm_hw_params (playback_handle, hw_params);
    snd_pcm_hw_params_free (hw_params);

    printf("snd_pcm_prepare\n");
        
    /* Prepare & Play */
    snd_pcm_prepare (playback_handle);

    printf("play\n");

    /* PLAY */
    int tot_pcm_samples = 0;
    while ((bytes_left >= 0) && (frame_size > 0)) {
      stream_pos += frame_size;
      bytes_left -= frame_size;

      //write(pcm, (const void *) sample_buf, info.audio_bytes);

      int pos = 0;
      int pcm_samples = (info.audio_bytes / 2);
      tot_pcm_samples += pcm_samples;
      //printf("[mp3dec %d pcm] ", info.audio_bytes/2);
#ifdef WRITE_DECODED_FILE
      int res = write(fdw, (const void *) sample_buf, info.audio_bytes);
      if (res != info.audio_bytes) {
        out("Error: cannot write pcm to file.\n");
        return 1;
      }
#endif
      while (pcm_samples) {
        int n = snd_pcm_writei (playback_handle, &sample_buf[pos], pcm_samples);
        pcm_samples -= n;
        pos += n;
      }
      //snd_pcm_start(playback_handle);
      
      // next buf
      frame_size = mp3_decode(mp3, stream_pos, bytes_left, sample_buf, NULL);
    }

    printf("Decoded MP3 files size %d into %d PCM samples\n", tot_mp3_size, tot_pcm_samples);

    // wait some time to let sound play...
    sleep(10);
    /* Close the handle and exit */
    snd_pcm_close (playback_handle);
#ifdef WRITE_DECODED_FILE
      int res = close(fdw);
      if (res < 0) {
        out("Error: cannot close write file.\n");
        return 1;
      }
#endif
    return 0;
}
