int DCTStream::getChar()
{
    if (current == limit)
        if (!readLine())
            return EOF;

    return *current++;
}