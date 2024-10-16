EncryptingStream::Create(const AP4_UI08* key, const AP4_UI08* iv, AP4_ByteStream* output, EncryptingStream*& stream) {
    stream = NULL;
    AP4_BlockCipher* block_cipher = NULL;
    AP4_Result result = AP4_DefaultBlockCipherFactory::Instance.CreateCipher(AP4_BlockCipher::AES_128,
                                                                             AP4_BlockCipher::ENCRYPT,
                                                                             AP4_BlockCipher::CBC,
                                                                             NULL,
                                                                             key,
                                                                             16,
                                                                             block_cipher);
    if (AP4_FAILED(result)) return result;
    AP4_CbcStreamCipher* stream_cipher = new AP4_CbcStreamCipher(block_cipher);
    stream_cipher->SetIV(iv);
    stream = new EncryptingStream(stream_cipher, output);
    
    return AP4_SUCCESS;
}