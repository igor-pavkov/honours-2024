int CLASS fgetc(FILE *stream) {
    int chr = ::fgetc(stream);
    if (stream==ifp) ifpProgress(1);
    return chr;
}