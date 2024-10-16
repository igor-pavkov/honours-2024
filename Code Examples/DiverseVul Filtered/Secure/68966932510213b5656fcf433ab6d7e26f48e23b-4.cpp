    PsdImage::PsdImage(BasicIo::AutoPtr io)
        : Image(ImageType::psd, mdExif | mdIptc | mdXmp, io)
    {
    } // PsdImage::PsdImage