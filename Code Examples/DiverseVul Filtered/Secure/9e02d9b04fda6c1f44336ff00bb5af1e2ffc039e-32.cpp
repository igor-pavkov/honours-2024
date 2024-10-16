void WavOutFile::writeHeader()
{
    WavHeader hdrTemp;
    int res;

    // swap byte order if necessary
    hdrTemp = header;
    _swap32((int &)hdrTemp.riff.package_len);
    _swap32((int &)hdrTemp.format.format_len);
    _swap16((short &)hdrTemp.format.fixed);
    _swap16((short &)hdrTemp.format.channel_number);
    _swap32((int &)hdrTemp.format.sample_rate);
    _swap32((int &)hdrTemp.format.byte_rate);
    _swap16((short &)hdrTemp.format.byte_per_sample);
    _swap16((short &)hdrTemp.format.bits_per_sample);
    _swap32((int &)hdrTemp.data.data_len);
    _swap32((int &)hdrTemp.fact.fact_len);
    _swap32((int &)hdrTemp.fact.fact_sample_len);
    
    // write the supplemented header in the beginning of the file
    fseek(fptr, 0, SEEK_SET);
    res = (int)fwrite(&hdrTemp, sizeof(hdrTemp), 1, fptr);
    if (res != 1)
    {
        ST_THROW_RT_ERROR("Error while writing to a wav file.");
    }

    // jump back to the end of the file
    fseek(fptr, 0, SEEK_END);
}