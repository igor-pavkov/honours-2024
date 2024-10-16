static size_t prvReadBytesFromBuffer( StreamBuffer_t * pxStreamBuffer,
                                      uint8_t * pucData,
                                      size_t xMaxCount,
                                      size_t xBytesAvailable )
{
    size_t xCount, xFirstLength, xNextTail;

    /* Use the minimum of the wanted bytes and the available bytes. */
    xCount = configMIN( xBytesAvailable, xMaxCount );

    if( xCount > ( size_t ) 0 )
    {
        xNextTail = pxStreamBuffer->xTail;

        /* Calculate the number of bytes that can be read - which may be
         * less than the number wanted if the data wraps around to the start of
         * the buffer. */
        xFirstLength = configMIN( pxStreamBuffer->xLength - xNextTail, xCount );

        /* Obtain the number of bytes it is possible to obtain in the first
         * read.  Asserts check bounds of read and write. */
        configASSERT( xFirstLength <= xMaxCount );
        configASSERT( ( xNextTail + xFirstLength ) <= pxStreamBuffer->xLength );
        ( void ) memcpy( ( void * ) pucData, ( const void * ) &( pxStreamBuffer->pucBuffer[ xNextTail ] ), xFirstLength ); /*lint !e9087 memcpy() requires void *. */

        /* If the total number of wanted bytes is greater than the number
         * that could be read in the first read... */
        if( xCount > xFirstLength )
        {
            /*...then read the remaining bytes from the start of the buffer. */
            configASSERT( xCount <= xMaxCount );
            ( void ) memcpy( ( void * ) &( pucData[ xFirstLength ] ), ( void * ) ( pxStreamBuffer->pucBuffer ), xCount - xFirstLength ); /*lint !e9087 memcpy() requires void *. */
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        /* Move the tail pointer to effectively remove the data read from
         * the buffer. */
        xNextTail += xCount;

        if( xNextTail >= pxStreamBuffer->xLength )
        {
            xNextTail -= pxStreamBuffer->xLength;
        }

        pxStreamBuffer->xTail = xNextTail;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    return xCount;
}