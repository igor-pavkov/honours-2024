uint WavInFile::getElapsedMS() const
{
    return (uint)(1000.0 * (double)dataRead / (double)header.format.byte_rate);
}