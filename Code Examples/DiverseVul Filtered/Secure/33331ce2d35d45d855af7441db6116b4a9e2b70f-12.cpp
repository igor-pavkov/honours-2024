    virtual AP4_Result Flush() {
        AP4_UI08 trailer[16];
        AP4_Size trailer_size = sizeof(trailer);
        AP4_Result result = m_StreamCipher->ProcessBuffer(NULL, 0, trailer, &trailer_size, true);
        if (AP4_SUCCEEDED(result) && trailer_size) {
            m_Output->Write(trailer, trailer_size);
            m_Size += 16-(m_Size%16);
        }
        
        return AP4_SUCCESS;
    }