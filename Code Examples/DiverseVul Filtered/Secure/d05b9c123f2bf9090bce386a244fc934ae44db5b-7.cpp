static size_t prvWriteMessageToBuffer( StreamBuffer_t * const pxStreamBuffer,
                                       const void * pvTxData,
                                       size_t xDataLengthBytes,
                                       size_t xSpace,
                                       size_t xRequiredSpace )
{
    BaseType_t xShouldWrite;
    size_t xReturn;

    if( xSpace == ( size_t ) 0 )
    {
        /* Doesn't matter if this is a stream buffer or a message buffer, there
         * is no space to write. */
        xShouldWrite = pdFALSE;
    }
    else if( ( pxStreamBuffer->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER ) == ( uint8_t ) 0 )
    {
        /* This is a stream buffer, as opposed to a message buffer, so writing a
         * stream of bytes rather than discrete messages.  Write as many bytes as
         * possible. */
        xShouldWrite = pdTRUE;
        xDataLengthBytes = configMIN( xDataLengthBytes, xSpace );
    }
    else if( xSpace >= xRequiredSpace )
    {
        /* This is a message buffer, as opposed to a stream buffer, and there
         * is enough space to write both the message length and the message itself
         * into the buffer.  Start by writing the length of the data, the data
         * itself will be written later in this function. */
        xShouldWrite = pdTRUE;
        ( void ) prvWriteBytesToBuffer( pxStreamBuffer, ( const uint8_t * ) &( xDataLengthBytes ), sbBYTES_TO_STORE_MESSAGE_LENGTH );
    }
    else
    {
        /* There is space available, but not enough space. */
        xShouldWrite = pdFALSE;
    }

    if( xShouldWrite != pdFALSE )
    {
        /* Writes the data itself. */
        xReturn = prvWriteBytesToBuffer( pxStreamBuffer, ( const uint8_t * ) pvTxData, xDataLengthBytes ); /*lint !e9079 Storage buffer is implemented as uint8_t for ease of sizing, alignment and access. */
    }
    else
    {
        xReturn = 0;
    }

    return xReturn;
}