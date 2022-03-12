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

struct wav_file_custom_header_data
{
   char header_name[5];
   unsigned num_bytes;
   char *data;
};

int read_wav_file(const char *file_name, struct wav_file *wav);

int read_wav_file_chdr(const char *file_name, struct wav_file *wav, struct wav_file_custom_header_data *chdr);

int free_wav_file(struct wav_file *wav);

int write_wav_file(const char *file_name, struct wav_file *wav);

int write_wav_file_chdr(const char *file_name, struct wav_file *wav, struct wav_file_custom_header_data *chdr);

int create_wav_file(struct wav_file *wav, unsigned num_frames, unsigned channels, unsigned bit_depth, unsigned sample_rate);

int wav_get_normalized(struct wav_file *wav, unsigned sample_idx, float *value);

int wav_set_normalized(struct wav_file *wav, unsigned sample_idx, float *value);

#endif
