BaseType_t xStreamBufferIsEmpty( StreamBufferHandle_t xStreamBuffer )
{
    const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    BaseType_t xReturn;
    size_t xTail;

    configASSERT( pxStreamBuffer );

    /* True if no bytes are available. */
    xTail = pxStreamBuffer->xTail;

    if( pxStreamBuffer->xHead == xTail )
    {
        xReturn = pdTRUE;
    }
    else
    {
        xReturn = pdFALSE;
    }

    return xReturn;
}