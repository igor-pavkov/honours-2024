uint WavInFile::getBytesPerSample() const
{
    return getNumChannels() * getNumBits() / 8;
}