TileBufferTask::execute ()
{
    try
    {
        //
        // Calculate information about the tile
        //

        Box2i tileRange = OPENEXR_IMF_INTERNAL_NAMESPACE::dataWindowForTile (
                _ifd->tileDesc,
                _ifd->minX, _ifd->maxX,
                _ifd->minY, _ifd->maxY,
                _tileBuffer->dx,
                _tileBuffer->dy,
                _tileBuffer->lx,
                _tileBuffer->ly);

        //
        // Get the size of the tile.
        //

        Array<unsigned int> numPixelsPerScanLine;
        numPixelsPerScanLine.resizeErase(tileRange.max.y - tileRange.min.y + 1);

        int sizeOfTile = 0;
        int maxBytesPerTileLine = 0;

        for (int y = tileRange.min.y; y <= tileRange.max.y; y++)
        {
            numPixelsPerScanLine[y - tileRange.min.y] = 0;

            int bytesPerLine = 0;

            for (int x = tileRange.min.x; x <= tileRange.max.x; x++)
            {
                int xOffset = _ifd->sampleCountXTileCoords * tileRange.min.x;
                int yOffset = _ifd->sampleCountYTileCoords * tileRange.min.y;

                int count = _ifd->getSampleCount(x - xOffset, y - yOffset);
                for (unsigned int c = 0; c < _ifd->slices.size(); ++c)
                {
                    // This slice does not exist in the file.
                    if ( !_ifd->slices[c]->fill)
                    {
                          sizeOfTile += count * pixelTypeSize(_ifd->slices[c]->typeInFile);
                          bytesPerLine += count * pixelTypeSize(_ifd->slices[c]->typeInFile);     
                    }
                }
                numPixelsPerScanLine[y - tileRange.min.y] += count;
            }

            if (bytesPerLine > maxBytesPerTileLine)
                maxBytesPerTileLine = bytesPerLine;
        }

        // (TODO) don't do this every time.
        if (_tileBuffer->compressor != 0)
            delete _tileBuffer->compressor;
        _tileBuffer->compressor = newTileCompressor
                                  (_ifd->header.compression(),
                                   maxBytesPerTileLine,
                                   _ifd->tileDesc.ySize,
                                   _ifd->header);

        //
        // Uncompress the data, if necessary
        //

        if (_tileBuffer->compressor && _tileBuffer->dataSize < static_cast<Int64>(sizeOfTile))
        {
            _tileBuffer->format = _tileBuffer->compressor->format();

            _tileBuffer->dataSize = _tileBuffer->compressor->uncompressTile
                (_tileBuffer->buffer, _tileBuffer->dataSize,
                 tileRange, _tileBuffer->uncompressedData);
        }
        else
        {
            //
            // If the line is uncompressed, it's in XDR format,
            // regardless of the compressor's output format.
            //

            _tileBuffer->format = Compressor::XDR;
            _tileBuffer->uncompressedData = _tileBuffer->buffer;
        }

	//
	// sanity check data size: the uncompressed data should be exactly 
	// 'sizeOfTile' (if it's less, the file is corrupt and there'll be a buffer overrun)
	//
        if (_tileBuffer->dataSize != static_cast<Int64>(sizeOfTile))
	{
		THROW (IEX_NAMESPACE::InputExc, "size mismatch when reading deep tile: expected " << sizeOfTile << "bytes of uncompressed data but got " << _tileBuffer->dataSize);
	}

        //
        // Convert the tile of pixel data back from the machine-independent
        // representation, and store the result in the frame buffer.
        //

        const char *readPtr = _tileBuffer->uncompressedData;
                                                        // points to where we
                                                        // read from in the
                                                        // tile block

        //
        // Iterate over the scan lines in the tile.
        //

        for (int y = tileRange.min.y; y <= tileRange.max.y; ++y)
        {
            //
            // Iterate over all image channels.
            //

            for (unsigned int i = 0; i < _ifd->slices.size(); ++i)
            {
                TInSliceInfo &slice = *_ifd->slices[i];

                //
                // These offsets are used to facilitate both
                // absolute and tile-relative pixel coordinates.
                //

                int xOffsetForData = (slice.xTileCoords == 0) ? 0 : tileRange.min.x;
                int yOffsetForData = (slice.yTileCoords == 0) ? 0 : tileRange.min.y;
                int xOffsetForSampleCount =
                        (_ifd->sampleCountXTileCoords == 0) ? 0 : tileRange.min.x;
                int yOffsetForSampleCount =
                        (_ifd->sampleCountYTileCoords == 0) ? 0 : tileRange.min.y;

                //
                // Fill the frame buffer with pixel data.
                //

                if (slice.skip)
                {
                    //
                    // The file contains data for this channel, but
                    // the frame buffer contains no slice for this channel.
                    //

                    skipChannel (readPtr, slice.typeInFile,
                                 numPixelsPerScanLine[y - tileRange.min.y]);
                }
                else
                {
                    //
                    // The frame buffer contains a slice for this channel.
                    //

                    copyIntoDeepFrameBuffer (readPtr, slice.pointerArrayBase,
                                             _ifd->sampleCountSliceBase,
                                             _ifd->sampleCountXStride,
                                             _ifd->sampleCountYStride,
                                             y,
                                             tileRange.min.x,
                                             tileRange.max.x,
                                             xOffsetForSampleCount, yOffsetForSampleCount,
                                             xOffsetForData, yOffsetForData,
                                             slice.sampleStride, 
                                             slice.xStride,
                                             slice.yStride,
                                             slice.fill,
                                             slice.fillValue, _tileBuffer->format,
                                             slice.typeInFrameBuffer,
                                             slice.typeInFile);
                }
            }
        }
    }
    catch (std::exception &e)
    {
        if (!_tileBuffer->hasException)
        {
            _tileBuffer->exception = e.what ();
            _tileBuffer->hasException = true;
        }
    }
    catch (...)
    {
        if (!_tileBuffer->hasException)
        {
            _tileBuffer->exception = "unrecognized exception";
            _tileBuffer->hasException = true;
        }
    }
}