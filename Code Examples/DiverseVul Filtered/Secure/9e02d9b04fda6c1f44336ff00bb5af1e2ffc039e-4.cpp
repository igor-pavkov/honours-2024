WavFileBase::~WavFileBase()
{
    delete[] convBuff;
    convBuffSize = 0;
}