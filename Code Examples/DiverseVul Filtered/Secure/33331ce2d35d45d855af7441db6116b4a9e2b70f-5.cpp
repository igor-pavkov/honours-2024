    virtual AP4_Result WritePartial(const void* buffer,
                                    AP4_Size    bytes_to_write, 
                                    AP4_Size&   bytes_written) {
        AP4_UI08* out = new AP4_UI08[bytes_to_write+16];
        AP4_Size  out_size = bytes_to_write+16;
        AP4_Result result = m_StreamCipher->ProcessBuffer((const AP4_UI08*)buffer,
                                                          bytes_to_write,
                                                          out,
                                                          &out_size);
        if (AP4_SUCCEEDED(result)) {
            result = m_Output->Write(out, out_size);
            bytes_written = bytes_to_write;
            m_Size       += bytes_to_write;
        } else {
            bytes_written = 0;
        }
        delete[] out;
        return result;
    }