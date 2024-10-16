int WavInFile::read(unsigned char *buffer, int maxElems)
{
    int numBytes;
    uint afterDataRead;

    // ensure it's 8 bit format
    if (header.format.bits_per_sample != 8)
    {
        ST_THROW_RT_ERROR("Error: WavInFile::read(char*, int) works only with 8bit samples.");
    }
    assert(sizeof(char) == 1);

    numBytes = maxElems;
    afterDataRead = dataRead + numBytes;
    if (afterDataRead > header.data.data_len) 
    {
        // Don't read more samples than are marked available in header
        numBytes = (int)header.data.data_len - (int)dataRead;
        assert(numBytes >= 0);
    }

    assert(buffer);
    numBytes = (int)fread(buffer, 1, numBytes, fptr);
    dataRead += numBytes;

    return numBytes;
}