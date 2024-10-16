static size_t prvBytesInBuffer( const StreamBuffer_t * const pxStreamBuffer )
{
/* Returns the distance between xTail and xHead. */
    size_t xCount;

    xCount = pxStreamBuffer->xLength + pxStreamBuffer->xHead;
    xCount -= pxStreamBuffer->xTail;

    if( xCount >= pxStreamBuffer->xLength )
    {
        xCount -= pxStreamBuffer->xLength;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    return xCount;
}