uint WavInFile::getLengthMS() const
{
    double numSamples;
    double sampleRate;

    numSamples = (double)getNumSamples();
    sampleRate = (double)getSampleRate();

    return (uint)(1000.0 * numSamples / sampleRate + 0.5);
}