SampleEncrypter::EncryptAudioSample(AP4_DataBuffer& sample, AP4_SampleDescription* sample_description)
{
    if (sample.GetDataSize() <= 16) {
        return AP4_SUCCESS;
    }
    
    // deal with E-AC3 syncframes
    if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_EC_3) {
        // syncword    : 16
        // strmtyp     : 2
        // substreamid : 3
        // frmsiz      : 11
        AP4_UI08*    data      = sample.UseData();
        unsigned int data_size = sample.GetDataSize();
        while (data_size > 4) {
            unsigned int syncword = (data[0]<<8) | data[1];
            if (syncword != 0x0b77) {
                return AP4_ERROR_INVALID_FORMAT; // unexpected!
            }
            unsigned int frmsiz = 1+(((data[2]<<8)|(data[3]))&0x7FF);
            unsigned int frame_size = 2*frmsiz;
            if (data_size < frame_size) {
                return AP4_ERROR_INVALID_FORMAT; // unexpected!
            }

            // encrypt the syncframe
            if (frame_size > 16) {
                unsigned int encrypted_block_count = (frame_size-16)/16;
                AP4_Size encrypted_size = encrypted_block_count*16;
                m_StreamCipher->SetIV(m_IV);
                m_StreamCipher->ProcessBuffer(data+16, encrypted_size, data+16, &encrypted_size);
            }
            
            data      += frame_size;
            data_size -= frame_size;
        }
    } else {
        unsigned int encrypted_block_count = (sample.GetDataSize()-16)/16;
        AP4_Size encrypted_size = encrypted_block_count*16;
        m_StreamCipher->SetIV(m_IV);
        m_StreamCipher->ProcessBuffer(sample.UseData()+16, encrypted_size, sample.UseData()+16, &encrypted_size);
    }
    
    return AP4_SUCCESS;
}