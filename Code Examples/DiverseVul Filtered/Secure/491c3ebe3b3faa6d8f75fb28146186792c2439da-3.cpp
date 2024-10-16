    Image::UniquePtr newPngInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new PngImage(std::move(io), create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }