WavOutFile::~WavOutFile()
{
    finishHeader();
    if (fptr) fclose(fptr);
    fptr = NULL;
}