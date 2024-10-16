    Image::AutoPtr newPsdInstance(BasicIo::AutoPtr io, bool /*create*/)
    {
        Image::AutoPtr image(new PsdImage(io));
        if (!image->good())
        {
            image.reset();
        }
        return image;
    }