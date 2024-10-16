int WavInFile::read(float *buffer, int maxElems)
{
    unsigned int afterDataRead;
    int numBytes;
    int numElems;
    int bytesPerSample;

    assert(buffer);

    bytesPerSample = header.format.bits_per_sample / 8;
    if ((bytesPerSample < 1) || (bytesPerSample > 4))
    {
        stringstream ss;
        ss << "\nOnly 8/16/24/32 bit sample WAV files supported. Can't open WAV file with ";
        ss << (int)header.format.bits_per_sample;
        ss << " bit sample format. ";
        ST_THROW_RT_ERROR(ss.str().c_str());
    }

    numBytes = maxElems * bytesPerSample;
    afterDataRead = dataRead + numBytes;
    if (afterDataRead > header.data.data_len) 
    {
        // Don't read more samples than are marked available in header
        numBytes = (int)header.data.data_len - (int)dataRead;
        assert(numBytes >= 0);
    }

    // read raw data into temporary buffer
    char *temp = (char*)getConvBuffer(numBytes);
    numBytes = (int)fread(temp, 1, numBytes, fptr);
    dataRead += numBytes;

    numElems = numBytes / bytesPerSample;

    // swap byte ordert & convert to float, depending on sample format
    switch (bytesPerSample)
    {
        case 1:
        {
            unsigned char *temp2 = (unsigned char*)temp;
            double conv = 1.0 / 128.0;
            for (int i = 0; i < numElems; i ++)
            {
                buffer[i] = (float)(temp2[i] * conv - 1.0);
            }
            break;
        }

        case 2:
        {
            short *temp2 = (short*)temp;
            double conv = 1.0 / 32768.0;
            for (int i = 0; i < numElems; i ++)
            {
                short value = temp2[i];
                buffer[i] = (float)(_swap16(value) * conv);
            }
            break;
        }

        case 3:
        {
            char *temp2 = (char *)temp;
            double conv = 1.0 / 8388608.0;
            for (int i = 0; i < numElems; i ++)
            {
                int value = *((int*)temp2);
                value = _swap32(value) & 0x00ffffff;             // take 24 bits
                value |= (value & 0x00800000) ? 0xff000000 : 0;  // extend minus sign bits
                buffer[i] = (float)(value * conv);
                temp2 += 3;
            }
            break;
        }

        case 4:
        {
            int *temp2 = (int *)temp;
            double conv = 1.0 / 2147483648.0;
            assert(sizeof(int) == 4);
            for (int i = 0; i < numElems; i ++)
            {
                int value = temp2[i];
                buffer[i] = (float)(_swap32(value) * conv);
            }
            break;
        }
    }

    return numElems;
}