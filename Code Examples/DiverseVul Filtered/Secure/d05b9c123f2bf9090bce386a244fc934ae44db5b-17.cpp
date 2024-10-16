static void prvInitialiseNewStreamBuffer( StreamBuffer_t * const pxStreamBuffer,
                                          uint8_t * const pucBuffer,
                                          size_t xBufferSizeBytes,
                                          size_t xTriggerLevelBytes,
                                          uint8_t ucFlags )
{
    /* Assert here is deliberately writing to the entire buffer to ensure it can
     * be written to without generating exceptions, and is setting the buffer to a
     * known value to assist in development/debugging. */
    #if ( configASSERT_DEFINED == 1 )
        {
            /* The value written just has to be identifiable when looking at the
             * memory.  Don't use 0xA5 as that is the stack fill value and could
             * result in confusion as to what is actually being observed. */
            const BaseType_t xWriteValue = 0x55;
            configASSERT( memset( pucBuffer, ( int ) xWriteValue, xBufferSizeBytes ) == pucBuffer );
        } /*lint !e529 !e438 xWriteValue is only used if configASSERT() is defined. */
    #endif

    ( void ) memset( ( void * ) pxStreamBuffer, 0x00, sizeof( StreamBuffer_t ) ); /*lint !e9087 memset() requires void *. */
    pxStreamBuffer->pucBuffer = pucBuffer;
    pxStreamBuffer->xLength = xBufferSizeBytes;
    pxStreamBuffer->xTriggerLevelBytes = xTriggerLevelBytes;
    pxStreamBuffer->ucFlags = ucFlags;
}