#pragma once
#include <string>
#include <stdexcept>
#include <map>
#include <tuple>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "wav_handler.h"

namespace wav_handler
{

    struct StereoSample
    {
        float ch0;
        float ch1;
    };

    class WaveFile
    {
        wav_file wav;
        std::map<std::string, std::string> custom_header_data;
        float sample_buf[16];

        WaveFile(const WaveFile &) = delete;
        WaveFile &operator=(const WaveFile &) = delete;
        WaveFile(WaveFile &&) = delete;
        WaveFile &operator=(WaveFile &&) = delete;

    public:
        WaveFile()
        {
            memset(&wav, 0, sizeof(wav_file));
        }

        ~WaveFile()
        {
            if (wav.data)
            {
                free_wav_file(&wav);
            }
        }

        void load_file(const std::string &fname, const std::vector<std::string> &custom_headers = {})
        {
            if (wav.data)
                throw std::runtime_error("file already loaded");
            std::vector<wav_file_custom_header_data> chdr_array;
            int i = 0;
            for (auto &hdr : custom_headers)
            {
                chdr_array.push_back({});
                chdr_array[i].data = nullptr;
                strcpy(chdr_array[i].header_name, hdr.substr(0, 4).c_str());
                chdr_array[i].num_bytes = 0;
                i++;
            }
            chdr_array.push_back({{0}, 0, nullptr});
            const auto err = read_wav_file_chdr(fname.c_str(), &wav, chdr_array.data());
            for (auto &hdr : chdr_array)
            {
                if (hdr.data)
                    custom_header_data[hdr.header_name] = std::string(hdr.data, hdr.num_bytes);
                free(hdr.data);
            }
            if (err)
                throw std::runtime_error("Error loading file" + fname);
            if (wav.channels > 16)
                throw std::range_error("Number of channels must be max 16");
        }

        void write_file(const std::string &fname, const std::map<std::string, std::string> custom_headers = {})
        {
            if (!wav.data)
                throw std::runtime_error("no file loaded");

            std::vector<wav_file_custom_header_data> chdr_array;
            int i = 0;
            for (auto &hdr : custom_headers)
            {
                chdr_array.push_back({});
                chdr_array[i].data = static_cast<char*>(calloc(hdr.second.size(), 1));
                memcpy(chdr_array[i].data, hdr.second.data(), hdr.second.size());
                strcpy(chdr_array[i].header_name, hdr.first.substr(0, 4).c_str());
                chdr_array[i].num_bytes = hdr.second.size();
                i++;
            }
            chdr_array.push_back({{0}, 0, nullptr});
            int err = write_wav_file_chdr(fname.c_str(), &wav, chdr_array.data());
            for (auto &hdr : chdr_array)
            {
                free(hdr.data);
            }
            if (err)
                throw std::runtime_error("Error writing file" + fname);
        }

        void create(unsigned length, bool stereo, unsigned bit_depth, unsigned sample_rate, bool is_float)
        {
            if (wav.data)
                throw std::runtime_error("file already loaded");
            auto err = create_wav_file(&wav, length, stereo ? 2 : 1, bit_depth, sample_rate);
            if (err)
                throw std::runtime_error("Error creating wav file");
            wav.is_float = is_float;
        }

        std::string get_header(const std::string &name)
        {
            auto data = custom_header_data.find(name);
            if (data == custom_header_data.end())
                throw std::invalid_argument("Header not found: " + name);
            return data->second;
        }

        unsigned get_length()
        {
            if (!wav.data)
                throw std::runtime_error("no file loaded");
            return wav.num_frames;
        }

        float get_sample(unsigned idx)
        {
            if (!wav.data)
                throw std::runtime_error("no file loaded");
            int err = wav_get_normalized(&wav, idx, sample_buf);
            if (err)
                throw std::runtime_error("error while getting sample at index " + std::to_string(idx));
            return sample_buf[0];
        }

        StereoSample get_sample_stereo(unsigned idx)
        {
            if (!wav.data)
                throw std::runtime_error("no file loaded");
            int err = wav_get_normalized(&wav, idx, sample_buf);
            if (err)
                throw std::runtime_error("error while getting sample at index " + std::to_string(idx));
            return {sample_buf[0], sample_buf[1]};
        }

        void set_sample(unsigned idx, float value)
        {
            if (!wav.data)
                throw std::runtime_error("no file loaded");
            sample_buf[0] = sample_buf[1] = value;
            int err = wav_set_normalized(&wav, idx, sample_buf);
            if (err)
                throw std::runtime_error("error while setting sample at index " + std::to_string(idx));
        }

        void set_sample_stereo(unsigned idx, const StereoSample &value)
        {
            if (!wav.data)
                throw std::runtime_error("no file loaded");
            sample_buf[0] = value.ch0;
            sample_buf[1] = value.ch1;
            int err = wav_set_normalized(&wav, idx, sample_buf);
            if (err)
                throw std::runtime_error("error while setting sample at index " + std::to_string(idx));
        }
    };

}