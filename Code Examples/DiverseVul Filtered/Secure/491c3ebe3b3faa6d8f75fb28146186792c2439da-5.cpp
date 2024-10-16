    PngImage::PngImage(BasicIo::UniquePtr io, bool create)
        : Image(ImageType::png, mdExif | mdIptc | mdXmp | mdComment, std::move(io))
    {
        if (create) {
            if (io_->open() == 0) {
#ifdef DEBUG
                std::cerr << "Exiv2::PngImage:: Creating PNG image to memory\n";
#endif
                IoCloser closer(*io_);
                if (io_->write(pngBlank, sizeof(pngBlank)) != sizeof(pngBlank)) {
#ifdef DEBUG
                    std::cerr << "Exiv2::PngImage:: Failed to create PNG image on memory\n";
#endif
                }
            }
        }
    }  // PngImage::PngImage