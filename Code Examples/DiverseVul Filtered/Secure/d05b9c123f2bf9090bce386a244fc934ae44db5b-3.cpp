static size_t prvWriteBytesToBuffer( StreamBuffer_t * const pxStreamBuffer,
                                     const uint8_t * pucData,
                                     size_t xCount )
{
    size_t xNextHead, xFirstLength;

    configASSERT( xCount > ( size_t ) 0 );

    xNextHead = pxStreamBuffer->xHead;

    /* Calculate the number of bytes that can be added in the first write -
     * which may be less than the total number of bytes that need to be added if
     * the buffer will wrap back to the beginning. */
    xFirstLength = configMIN( pxStreamBuffer->xLength - xNextHead, xCount );

    /* Write as many bytes as can be written in the first write. */
    configASSERT( ( xNextHead + xFirstLength ) <= pxStreamBuffer->xLength );
    ( void ) memcpy( ( void * ) ( &( pxStreamBuffer->pucBuffer[ xNextHead ] ) ), ( const void * ) pucData, xFirstLength ); /*lint !e9087 memcpy() requires void *. */

    /* If the number of bytes written was less than the number that could be
     * written in the first write... */
    if( xCount > xFirstLength )
    {
        /* ...then write the remaining bytes to the start of the buffer. */
        configASSERT( ( xCount - xFirstLength ) <= pxStreamBuffer->xLength );
        ( void ) memcpy( ( void * ) pxStreamBuffer->pucBuffer, ( const void * ) &( pucData[ xFirstLength ] ), xCount - xFirstLength ); /*lint !e9087 memcpy() requires void *. */
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    xNextHead += xCount;

    if( xNextHead >= pxStreamBuffer->xLength )
    {
        xNextHead -= pxStreamBuffer->xLength;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    pxStreamBuffer->xHead = xNextHead;

    return xCount;
}