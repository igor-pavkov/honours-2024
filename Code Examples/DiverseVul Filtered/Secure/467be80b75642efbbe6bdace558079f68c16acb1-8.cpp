DeepTiledInputFile::setFrameBuffer (const DeepFrameBuffer &frameBuffer)
{
#if ILMBASE_THREADING_ENABLED
    std::lock_guard<std::mutex> lock (*_data->_streamData);
#endif
    //
    // Set the frame buffer
    //

    //
    // Check if the new frame buffer descriptor is
    // compatible with the image file header.
    //

    const ChannelList &channels = _data->header.channels();

    for (DeepFrameBuffer::ConstIterator j = frameBuffer.begin();
         j != frameBuffer.end();
         ++j)
    {
        ChannelList::ConstIterator i = channels.find (j.name());

        if (i == channels.end())
            continue;

        if (i.channel().xSampling != j.slice().xSampling ||
            i.channel().ySampling != j.slice().ySampling)
            THROW (IEX_NAMESPACE::ArgExc, "X and/or y subsampling factors "
                                "of \"" << i.name() << "\" channel "
                                "of input file \"" << fileName() << "\" are "
                                "not compatible with the frame buffer's "
                                "subsampling factors.");
    }

    //
    // Store the pixel sample count table.
    // (TODO) Support for different sampling rates?
    //

    const Slice& sampleCountSlice = frameBuffer.getSampleCountSlice();
    if (sampleCountSlice.base == 0)
    {
        throw IEX_NAMESPACE::ArgExc ("Invalid base pointer, please set a proper sample count slice.");
    }
    else
    {
        _data->sampleCountSliceBase = sampleCountSlice.base;
        _data->sampleCountXStride = sampleCountSlice.xStride;
        _data->sampleCountYStride = sampleCountSlice.yStride;
        _data->sampleCountXTileCoords = sampleCountSlice.xTileCoords;
        _data->sampleCountYTileCoords = sampleCountSlice.yTileCoords;
    }

    //
    // Initialize the slice table for readPixels().
    //

    vector<TInSliceInfo*> slices;
    ChannelList::ConstIterator i = channels.begin();

    for (DeepFrameBuffer::ConstIterator j = frameBuffer.begin();
         j != frameBuffer.end();
         ++j)
    {
        while (i != channels.end() && strcmp (i.name(), j.name()) < 0)
        {
            //
            // Channel i is present in the file but not
            // in the frame buffer; data for channel i
            // will be skipped during readPixels().
            //

            slices.push_back (new TInSliceInfo (i.channel().type,
                                                NULL,
                                                i.channel().type,
                                                0,      // xStride
                                                0,      // yStride
                                                0,      // sampleStride
                                                false,  // fill
                                                true,   // skip
                                                0.0));  // fillValue
            ++i;
        }

        bool fill = false;

        if (i == channels.end() || strcmp (i.name(), j.name()) > 0)
        {
            //
            // Channel i is present in the frame buffer, but not in the file.
            // In the frame buffer, slice j will be filled with a default value.
            //

            fill = true;
        }

        slices.push_back (new TInSliceInfo (j.slice().type,
                                            j.slice().base,
                                            fill? j.slice().type: i.channel().type,
                                            j.slice().xStride,
                                            j.slice().yStride,
                                            j.slice().sampleStride,
                                            fill,
                                            false, // skip
                                            j.slice().fillValue,
                                            (j.slice().xTileCoords)? 1: 0,
                                            (j.slice().yTileCoords)? 1: 0));


        if (i != channels.end() && !fill)
            ++i;
    }

    // (TODO) inspect the following code. It's additional to the scanline input file.
    // Is this needed?

    while (i != channels.end())
    {
        //
        // Channel i is present in the file but not
        // in the frame buffer; data for channel i
        // will be skipped during readPixels().
        //

        slices.push_back (new TInSliceInfo (i.channel().type,
                                            NULL,
                                            i.channel().type,
                                            0, // xStride
                                            0, // yStride
                                            0, // sampleStride
                                            false,  // fill
                                            true, // skip
                                            0.0)); // fillValue
        ++i;
    }

    //
    // Store the new frame buffer.
    //

    _data->frameBuffer = frameBuffer;

    for (size_t i = 0; i < _data->slices.size(); i++)
        delete _data->slices[i];
    _data->slices = slices;
}