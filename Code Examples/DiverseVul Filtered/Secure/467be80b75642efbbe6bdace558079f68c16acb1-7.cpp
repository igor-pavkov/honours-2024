DeepTiledInputFile::rawTileData (int &dx, int &dy,
                             int &lx, int &ly,
                             char * pixelData,
                             Int64 &pixelDataSize) const
{
     if (!isValidTile (dx, dy, lx, ly))
               throw IEX_NAMESPACE::ArgExc ("Tried to read a tile outside "
                                   "the image file's data window.");
    
     Int64 tileOffset = _data->tileOffsets (dx, dy, lx, ly);
                                   
     if(tileOffset == 0)
     {
        THROW (IEX_NAMESPACE::InputExc, "Tile (" << dx << ", " << dy << ", " <<
        lx << ", " << ly << ") is missing.");
     }
     
#if ILMBASE_THREADING_ENABLED
     std::lock_guard<std::mutex> lock(*_data->_streamData);
#endif
     if (_data->_streamData->is->tellg() != tileOffset)
                                          _data->_streamData->is->seekg (tileOffset);
                                   
     
     //
     // Read the first few bytes of the tile (the header).
     // Verify that the tile coordinates and the level number
     // are correct.
     //
     
     int tileXCoord, tileYCoord, levelX, levelY;
     
     if (isMultiPart(_data->version))
     {
         int partNumber;
         Xdr::read <StreamIO> (*_data->_streamData->is, partNumber);
         if (partNumber != _data->partNumber)
         {
             THROW (IEX_NAMESPACE::ArgExc, "Unexpected part number " << partNumber
             << ", should be " << _data->partNumber << ".");
         }
     }
     
     Xdr::read <StreamIO> (*_data->_streamData->is, tileXCoord);
     Xdr::read <StreamIO> (*_data->_streamData->is, tileYCoord);
     Xdr::read <StreamIO> (*_data->_streamData->is, levelX);
     Xdr::read <StreamIO> (*_data->_streamData->is, levelY);
     
     Int64 sampleCountTableSize;
     Int64 packedDataSize;
     Xdr::read <StreamIO> (*_data->_streamData->is, sampleCountTableSize);
     
     Xdr::read <StreamIO> (*_data->_streamData->is, packedDataSize);
     
          
     
     if (tileXCoord != dx)
         throw IEX_NAMESPACE::InputExc ("Unexpected tile x coordinate.");
     
     if (tileYCoord != dy)
         throw IEX_NAMESPACE::InputExc ("Unexpected tile y coordinate.");
     
     if (levelX != lx)
         throw IEX_NAMESPACE::InputExc ("Unexpected tile x level number coordinate.");
     
     if (levelY != ly)
         throw IEX_NAMESPACE::InputExc ("Unexpected tile y level number coordinate.");
     
     
     // total requirement for reading all the data
     
     Int64 totalSizeRequired=40+sampleCountTableSize+packedDataSize;
     
     bool big_enough = totalSizeRequired<=pixelDataSize;
     
     pixelDataSize = totalSizeRequired;
     
     // was the block we were given big enough?
     if(!big_enough || pixelData==NULL)
     {        
         // special case: seek stream back to start if we are at the beginning (regular reading pixels assumes it doesn't need to seek
         // in single part files)
         if(!isMultiPart(_data->version))
         {
             _data->_streamData->is->seekg(_data->_streamData->currentPosition); 
         }
         // leave lock here - bail before reading more data
         return;
     }
     
     // copy the values we have read into the output block
     *(int *) (pixelData+0) = dx;
     *(int *) (pixelData+4) = dy;
     *(int *) (pixelData+8) = levelX;
     *(int *) (pixelData+12) = levelY;
     *(Int64 *) (pixelData+16) =sampleCountTableSize;
     *(Int64 *) (pixelData+24) = packedDataSize;
     
     // didn't read the unpackedsize - do that now
     Xdr::read<StreamIO> (*_data->_streamData->is, *(Int64 *) (pixelData+32));
     
     // read the actual data
     _data->_streamData->is->read(pixelData+40, sampleCountTableSize+packedDataSize);
     
     
     if(!isMultiPart(_data->version))
     {
         _data->_streamData->currentPosition+=sampleCountTableSize+packedDataSize+40;
     }
     
     // leave lock here
     
     
}