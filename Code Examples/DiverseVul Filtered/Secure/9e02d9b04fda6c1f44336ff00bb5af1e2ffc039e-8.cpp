WavOutFile::WavOutFile(FILE *file, int sampleRate, int bits, int channels)
{
    bytesWritten = 0;
    fptr = file;
    if (fptr == NULL) 
    {
        string msg = "Error : Unable to access output file stream.";
        ST_THROW_RT_ERROR(msg.c_str());
    }

    fillInHeader(sampleRate, bits, channels);
    writeHeader();
}