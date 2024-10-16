GooString *DCTStream::getPSFilter(int psLevel, const char *indent)
{
    GooString *s;

    if (psLevel < 2) {
        return nullptr;
    }
    if (!(s = str->getPSFilter(psLevel, indent))) {
        return nullptr;
    }
    s->append(indent)->append("<< >> /DCTDecode filter\n");
    return s;
}