int WavInFile::eof() const
{
    // return true if all data has been read or file eof has reached
    return (dataRead == header.data.data_len || feof(fptr));
}