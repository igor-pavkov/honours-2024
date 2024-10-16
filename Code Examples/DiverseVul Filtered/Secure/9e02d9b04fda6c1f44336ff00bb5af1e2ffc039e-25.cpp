uint WavInFile::getNumSamples() const
{
    if (header.format.byte_per_sample == 0) return 0;
    if (header.format.fixed > 1) return header.fact.fact_sample_len;
    return header.data.data_len / (unsigned short)header.format.byte_per_sample;
}