    Image::AutoPtr newWebPInstance(BasicIo::AutoPtr io, bool /*create*/)
    {
        Image::AutoPtr image(new WebPImage(io));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }