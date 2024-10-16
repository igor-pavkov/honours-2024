    SampleEncrypter(AP4_CbcStreamCipher* stream_cipher, const AP4_UI08* iv):
        m_StreamCipher(stream_cipher) {
        AP4_CopyMemory(m_IV, iv, 16);
    }