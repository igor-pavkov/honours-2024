DeepTiledInputFile::readPixelSampleCounts (int dx1, int dx2,
                                           int dy1, int dy2,
                                           int lx,  int ly)
{
    Int64 savedFilePos = 0;

    try
    {
#if ILMBASE_THREADING_ENABLED
        std::lock_guard<std::mutex> lock (*_data->_streamData);
#endif
        savedFilePos = _data->_streamData->is->tellg();

        
        if (!isValidLevel (lx, ly))
        {
            THROW (IEX_NAMESPACE::ArgExc,
                   "Level coordinate "
                   "(" << lx << ", " << ly << ") "
                   "is invalid.");
        }
        
        if (dx1 > dx2)
            std::swap (dx1, dx2);

        if (dy1 > dy2)
            std::swap (dy1, dy2);

        int dyStart = dy1;
        int dyStop  = dy2 + 1;
        int dY      = 1;

        if (_data->lineOrder == DECREASING_Y)
        {
            dyStart = dy2;
            dyStop  = dy1 - 1;
            dY      = -1;
        }

        // (TODO) Check if we have read the sample counts for those tiles,
        // if we have, no need to read again.
        for (int dy = dyStart; dy != dyStop; dy += dY)
        {
            for (int dx = dx1; dx <= dx2; dx++)
            {
                
                if (!isValidTile (dx, dy, lx, ly))
                {
                    THROW (IEX_NAMESPACE::ArgExc,
                           "Tile (" << dx << ", " << dy << ", " <<
                           lx << "," << ly << ") is not a valid tile.");
                }
                
                Box2i tileRange = OPENEXR_IMF_INTERNAL_NAMESPACE::dataWindowForTile (
                        _data->tileDesc,
                        _data->minX, _data->maxX,
                        _data->minY, _data->maxY,
                        dx, dy, lx, ly);

                int xOffset = _data->sampleCountXTileCoords * tileRange.min.x;
                int yOffset = _data->sampleCountYTileCoords * tileRange.min.y;

                //
                // Skip and check the tile coordinates.
                //

                _data->_streamData->is->seekg(_data->tileOffsets(dx, dy, lx, ly));

                if (isMultiPart(_data->version))
                {
                    int partNumber;
                    Xdr::read <StreamIO> (*_data->_streamData->is, partNumber);

                    if (partNumber != _data->partNumber)
                        throw IEX_NAMESPACE::InputExc ("Unexpected part number.");
                }

                int xInFile, yInFile, lxInFile, lyInFile;
                Xdr::read <StreamIO> (*_data->_streamData->is, xInFile);
                Xdr::read <StreamIO> (*_data->_streamData->is, yInFile);
                Xdr::read <StreamIO> (*_data->_streamData->is, lxInFile);
                Xdr::read <StreamIO> (*_data->_streamData->is, lyInFile);

                if (xInFile != dx)
                    throw IEX_NAMESPACE::InputExc ("Unexpected tile x coordinate.");

                if (yInFile != dy)
                    throw IEX_NAMESPACE::InputExc ("Unexpected tile y coordinate.");

                if (lxInFile != lx)
                    throw IEX_NAMESPACE::InputExc ("Unexpected tile x level number coordinate.");

                if (lyInFile != ly)
                    throw IEX_NAMESPACE::InputExc ("Unexpected tile y level number coordinate.");

                Int64 tableSize, dataSize, unpackedDataSize;
                Xdr::read <StreamIO> (*_data->_streamData->is, tableSize);
                Xdr::read <StreamIO> (*_data->_streamData->is, dataSize);
                Xdr::read <StreamIO> (*_data->_streamData->is, unpackedDataSize);

                
                if(tableSize>_data->maxSampleCountTableSize)
                {
                    THROW (IEX_NAMESPACE::ArgExc, "Bad sampleCountTableDataSize read from tile "<< dx << ',' << dy << ',' << lx << ',' << ly << ": expected " << _data->maxSampleCountTableSize << " or less, got "<< tableSize);
                }
                    
                
                //
                // We make a check on the data size requirements here.
                // Whilst we wish to store 64bit sizes on disk, not all the compressors
                // have been made to work with such data sizes and are still limited to
                // using signed 32 bit (int) for the data size. As such, this version
                // insists that we validate that the data size does not exceed the data
                // type max limit.
                // @TODO refactor the compressor code to ensure full 64-bit support.
                //

                Int64 compressorMaxDataSize = static_cast<Int64>(std::numeric_limits<int>::max());
                if (dataSize         > compressorMaxDataSize ||
                    unpackedDataSize > compressorMaxDataSize ||
                    tableSize        > compressorMaxDataSize)
                {
                    THROW (IEX_NAMESPACE::ArgExc, "This version of the library does not"
                          << "support the allocation of data with size  > "
                          << compressorMaxDataSize
                          << " file table size    :" << tableSize
                          << " file unpacked size :" << unpackedDataSize
                          << " file packed size   :" << dataSize << ".\n");
                }

                //
                // Read and uncompress the pixel sample count table.
                //

                _data->_streamData->is->read(_data->sampleCountTableBuffer, tableSize);

                const char* readPtr;

                if (tableSize < _data->maxSampleCountTableSize)
                {
                    if(!_data->sampleCountTableComp)
                    {
                        THROW(IEX_NAMESPACE::ArgExc,"Deep scanline data corrupt at tile " << dx << ',' << dy << ',' << lx << ',' <<  ly << " (sampleCountTableDataSize error)");
                    }
                    _data->sampleCountTableComp->uncompress(_data->sampleCountTableBuffer,
                                                            tableSize,
                                                            tileRange.min.y,
                                                            readPtr);
                }
                else
                    readPtr = _data->sampleCountTableBuffer;

                size_t cumulative_total_samples =0;
                int lastAccumulatedCount;
                for (int j = tileRange.min.y; j <= tileRange.max.y; j++)
                {
                    lastAccumulatedCount = 0;
                    for (int i = tileRange.min.x; i <= tileRange.max.x; i++)
                    {
                        int accumulatedCount;
                        Xdr::read <CharPtrIO> (readPtr, accumulatedCount);
                        
                        if (accumulatedCount < lastAccumulatedCount)
                        {
                            THROW(IEX_NAMESPACE::ArgExc,"Deep tile sampleCount data corrupt at tile " 
                                  << dx << ',' << dy << ',' << lx << ',' <<  ly << " (negative sample count detected)");
                        }

                        int count = accumulatedCount - lastAccumulatedCount;
                        lastAccumulatedCount = accumulatedCount;
                        
                        _data->getSampleCount(i - xOffset, j - yOffset) =count;
                    }
                    cumulative_total_samples += lastAccumulatedCount;
                }
                
                if(cumulative_total_samples * _data->combinedSampleSize > unpackedDataSize)
                {
                    THROW(IEX_NAMESPACE::ArgExc,"Deep scanline sampleCount data corrupt at tile " 
                                                << dx << ',' << dy << ',' << lx << ',' <<  ly 
                                                << ": pixel data only contains " << unpackedDataSize 
                                                << " bytes of data but table references at least " 
                                                << cumulative_total_samples*_data->combinedSampleSize << " bytes of sample data" );            
                }
                    
            }
        }

        _data->_streamData->is->seekg(savedFilePos);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        REPLACE_EXC (e, "Error reading sample count data from image "
                     "file \"" << fileName() << "\". " << e.what());

         _data->_streamData->is->seekg(savedFilePos);

        throw;
    }
}