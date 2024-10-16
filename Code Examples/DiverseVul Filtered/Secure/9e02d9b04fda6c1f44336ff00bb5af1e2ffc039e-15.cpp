WavInFile::WavInFile(const char *fileName)
{
    // Try to open the file for reading
    fptr = fopen(fileName, "rb");
    if (fptr == NULL) 
    {
        // didn't succeed
        string msg = "Error : Unable to open file \"";
        msg += fileName;
        msg += "\" for reading.";
        ST_THROW_RT_ERROR(msg.c_str());
    }

    init();
}