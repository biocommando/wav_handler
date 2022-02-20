#ifndef WAV_HANDLER_H
#define WAV_HANDLER_H
struct wav_file
{
   unsigned num_bytes;
   unsigned channels;
   unsigned num_frames; // num_bytes / (channels * byte_per_sample)
   unsigned sample_rate;
   unsigned bit_depth;
   int is_float;
   char *data;
};

int read_wav_file(const char *file_name, struct wav_file *wav);

int free_wav_file(struct wav_file *wav);

int write_wav_file(const char *file_name, struct wav_file *wav);

int create_wav_file(struct wav_file *wav, unsigned num_frames, unsigned channels, unsigned bit_depth, unsigned sample_rate);

int wav_get_normalized(struct wav_file *wav, unsigned sample_idx, float *value);

int wav_set_normalized(struct wav_file *wav, unsigned sample_idx, float *value);

#endif
