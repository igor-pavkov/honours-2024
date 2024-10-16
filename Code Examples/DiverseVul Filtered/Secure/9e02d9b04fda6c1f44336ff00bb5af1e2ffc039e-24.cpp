int WavInFile::readWavHeaders()
{
    int res;

    memset(&header, 0, sizeof(header));

    res = readRIFFBlock();
    if (res) return 1;
    // read header blocks until data block is found
    do
    {
        // read header blocks
        res = readHeaderBlock();
        if (res < 0) return 1;  // error in file structure
    } while (res == 0);
    // check that all required tags are legal
    return checkCharTags();
}