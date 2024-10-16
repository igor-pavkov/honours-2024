PackedAudioWriter::WriteSample(AP4_Sample&            sample,
                               AP4_DataBuffer&        sample_data,
                               AP4_SampleDescription* sample_description,
                               AP4_ByteStream&        output)
{
    AP4_AudioSampleDescription* audio_desc = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sample_description);
    if (audio_desc == NULL) {
        return AP4_ERROR_INVALID_FORMAT;
    }
    if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_MP4A) {
        AP4_MpegAudioSampleDescription* mpeg_audio_desc = AP4_DYNAMIC_CAST(AP4_MpegAudioSampleDescription, sample_description);

        if (mpeg_audio_desc == NULL) return AP4_ERROR_NOT_SUPPORTED;
        if (mpeg_audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_AAC_LC   &&
            mpeg_audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_AAC_MAIN &&
            mpeg_audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_SBR      &&
            mpeg_audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_PS) {
            return AP4_ERROR_NOT_SUPPORTED;
        }

        unsigned int sample_rate   = mpeg_audio_desc->GetSampleRate();
        unsigned int channel_count = mpeg_audio_desc->GetChannelCount();
        const AP4_DataBuffer& dsi  = mpeg_audio_desc->GetDecoderInfo();
        if (dsi.GetDataSize()) {
            AP4_Mp4AudioDecoderConfig dec_config;
            AP4_Result result = dec_config.Parse(dsi.GetData(), dsi.GetDataSize());
            if (AP4_SUCCEEDED(result)) {
                sample_rate = dec_config.m_SamplingFrequency;
                channel_count = dec_config.m_ChannelCount;
            }
        }
        unsigned int sampling_frequency_index = GetSamplingFrequencyIndex(sample_rate);
        unsigned int channel_configuration    = channel_count;

        WriteAdtsHeader(output, sample.GetSize(), sampling_frequency_index, channel_configuration);
    } else if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AC_4) {
        WriteAc4Header(output, sample.GetSize());
    }
    return output.Write(sample_data.GetData(), sample_data.GetDataSize());
}