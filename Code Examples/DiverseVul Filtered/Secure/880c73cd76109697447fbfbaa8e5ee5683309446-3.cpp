static int calc_deflate_block_size(int tmpblock_size)
{
    z_stream zstream;
    int size;

    zstream.zalloc = Z_NULL;
    zstream.zfree  = Z_NULL;
    zstream.opaque = Z_NULL;
    if (deflateInit(&zstream, 0) != Z_OK)
        return -1;
    size = deflateBound(&zstream, tmpblock_size);
    deflateEnd(&zstream);

    return size;
}