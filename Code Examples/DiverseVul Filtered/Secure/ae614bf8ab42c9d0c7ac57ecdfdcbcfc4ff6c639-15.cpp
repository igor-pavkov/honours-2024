DCTStream::~DCTStream()
{
    jpeg_destroy_decompress(&cinfo);
    delete str;
}