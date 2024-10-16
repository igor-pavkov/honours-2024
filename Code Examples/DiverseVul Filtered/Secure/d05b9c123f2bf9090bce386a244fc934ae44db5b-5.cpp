static size_t prvReadMessageFromBuffer( StreamBuffer_t * pxStreamBuffer,
                                        void * pvRxData,
                                        size_t xBufferLengthBytes,
                                        size_t xBytesAvailable,
                                        size_t xBytesToStoreMessageLength )
{
    size_t xOriginalTail, xReceivedLength, xNextMessageLength;
    configMESSAGE_BUFFER_LENGTH_TYPE xTempNextMessageLength;

    if( xBytesToStoreMessageLength != ( size_t ) 0 )
    {
        /* A discrete message is being received.  First receive the length
         * of the message.  A copy of the tail is stored so the buffer can be
         * returned to its prior state if the length of the message is too
         * large for the provided buffer. */
        xOriginalTail = pxStreamBuffer->xTail;
        ( void ) prvReadBytesFromBuffer( pxStreamBuffer, ( uint8_t * ) &xTempNextMessageLength, xBytesToStoreMessageLength, xBytesAvailable );
        xNextMessageLength = ( size_t ) xTempNextMessageLength;

        /* Reduce the number of bytes available by the number of bytes just
         * read out. */
        xBytesAvailable -= xBytesToStoreMessageLength;

        /* Check there is enough space in the buffer provided by the
         * user. */
        if( xNextMessageLength > xBufferLengthBytes )
        {
            /* The user has provided insufficient space to read the message
             * so return the buffer to its previous state (so the length of
             * the message is in the buffer again). */
            pxStreamBuffer->xTail = xOriginalTail;
            xNextMessageLength = 0;
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
    else
    {
        /* A stream of bytes is being received (as opposed to a discrete
         * message), so read as many bytes as possible. */
        xNextMessageLength = xBufferLengthBytes;
    }

    /* Read the actual data. */
    xReceivedLength = prvReadBytesFromBuffer( pxStreamBuffer, ( uint8_t * ) pvRxData, xNextMessageLength, xBytesAvailable ); /*lint !e9079 Data storage area is implemented as uint8_t array for ease of sizing, indexing and alignment. */

    return xReceivedLength;
}