void vStreamBufferDelete( StreamBufferHandle_t xStreamBuffer )
{
    StreamBuffer_t * pxStreamBuffer = xStreamBuffer;

    configASSERT( pxStreamBuffer );

    traceSTREAM_BUFFER_DELETE( xStreamBuffer );

    if( ( pxStreamBuffer->ucFlags & sbFLAGS_IS_STATICALLY_ALLOCATED ) == ( uint8_t ) pdFALSE )
    {
        #if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
            {
                /* Both the structure and the buffer were allocated using a single call
                * to pvPortMalloc(), hence only one call to vPortFree() is required. */
                vPortFree( ( void * ) pxStreamBuffer ); /*lint !e9087 Standard free() semantics require void *, plus pxStreamBuffer was allocated by pvPortMalloc(). */
            }
        #else
            {
                /* Should not be possible to get here, ucFlags must be corrupt.
                 * Force an assert. */
                configASSERT( xStreamBuffer == ( StreamBufferHandle_t ) ~0 );
            }
        #endif
    }
    else
    {
        /* The structure and buffer were not allocated dynamically and cannot be
         * freed - just scrub the structure so future use will assert. */
        ( void ) memset( pxStreamBuffer, 0x00, sizeof( StreamBuffer_t ) );
    }
}