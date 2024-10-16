int DCTStream::lookChar()
{
    if (unlikely(current == nullptr)) {
        return EOF;
    }
    return *current;
}