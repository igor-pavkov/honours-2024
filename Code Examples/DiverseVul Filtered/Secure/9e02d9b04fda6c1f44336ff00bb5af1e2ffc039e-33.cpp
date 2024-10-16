WavInFile::~WavInFile()
{
    if (fptr) fclose(fptr);
    fptr = NULL;
}