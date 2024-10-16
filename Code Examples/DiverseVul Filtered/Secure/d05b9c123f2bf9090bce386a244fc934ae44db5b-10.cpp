size_t xStreamBufferSpacesAvailable( StreamBufferHandle_t xStreamBuffer )
{
    const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xSpace;

    configASSERT( pxStreamBuffer );

    xSpace = pxStreamBuffer->xLength + pxStreamBuffer->xTail;
    xSpace -= pxStreamBuffer->xHead;
    xSpace -= ( size_t ) 1;

    if( xSpace >= pxStreamBuffer->xLength )
    {
        xSpace -= pxStreamBuffer->xLength;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    return xSpace;
}