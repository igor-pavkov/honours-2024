    void Image::setIccProfile(Exiv2::DataBuf& iccProfile,bool bTestValid)
    {
        if ( bTestValid ) {
            if ( iccProfile.pData_ && ( iccProfile.size_ < (long) sizeof(long)) ) throw Error(kerInvalidIccProfile);
            long size = iccProfile.pData_ ? getULong(iccProfile.pData_, bigEndian): -1;
            if ( size!= iccProfile.size_ ) throw Error(kerInvalidIccProfile);
        }
        iccProfile_ = iccProfile;
    }