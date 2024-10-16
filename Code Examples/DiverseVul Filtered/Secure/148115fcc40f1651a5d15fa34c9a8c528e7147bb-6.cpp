static void* OGRExpatMalloc( size_t size )
{
    if( CanAlloc(size) )
        return malloc(size);

    return nullptr;
}