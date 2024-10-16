void *WavFileBase::getConvBuffer(int sizeBytes)
{
    if (convBuffSize < sizeBytes)
    {
        delete[] convBuff;

        convBuffSize = (sizeBytes + 15) & -8;   // round up to following 8-byte bounday
        convBuff = new char[convBuffSize];
    }
    return convBuff;
}