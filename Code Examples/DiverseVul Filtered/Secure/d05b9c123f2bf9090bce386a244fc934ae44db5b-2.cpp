    uint8_t ucStreamBufferGetStreamBufferType( StreamBufferHandle_t xStreamBuffer )
    {
        return( xStreamBuffer->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER );
    }