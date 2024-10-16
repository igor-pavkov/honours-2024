void WavInFile::rewind()
{
    int hdrsOk;

    fseek(fptr, 0, SEEK_SET);
    hdrsOk = readWavHeaders();
    assert(hdrsOk == 0);
    dataRead = 0;
}