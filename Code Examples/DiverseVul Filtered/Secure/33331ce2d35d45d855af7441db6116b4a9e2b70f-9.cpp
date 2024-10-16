    EncryptingStream(AP4_CbcStreamCipher* stream_cipher, AP4_ByteStream* output):
        m_ReferenceCount(1),
        m_StreamCipher(stream_cipher),
        m_Output(output),
        m_Size(0) {
        output->AddReference();
    }