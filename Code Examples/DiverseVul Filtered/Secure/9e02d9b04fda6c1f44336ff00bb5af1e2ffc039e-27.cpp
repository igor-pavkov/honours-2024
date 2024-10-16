WavInFile::WavInFile(FILE *file)
{
    // Try to open the file for reading
    fptr = file;
    if (!file) 
    {
        // didn't succeed
        string msg = "Error : Unable to access input stream for reading";
        ST_THROW_RT_ERROR(msg.c_str());
    }

    init();
}