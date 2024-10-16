int WavInFile::readRIFFBlock()
{
    if (fread(&(header.riff), sizeof(WavRiff), 1, fptr) != 1) return -1;

    // swap 32bit data byte order if necessary
    _swap32((int &)header.riff.package_len);

    // header.riff.riff_char should equal to 'RIFF');
    if (memcmp(riffStr, header.riff.riff_char, 4) != 0) return -1;
    // header.riff.wave should equal to 'WAVE'
    if (memcmp(waveStr, header.riff.wave, 4) != 0) return -1;

    return 0;
}