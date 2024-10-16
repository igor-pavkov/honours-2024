DeepTiledInputFile::Data::validateStreamSize()
{
    const Box2i &dataWindow = header.dataWindow();
    Int64 tileWidth = header.tileDescription().xSize;
    Int64 tileHeight = header.tileDescription().ySize;

    Int64 tilesX = (static_cast<Int64>(dataWindow.max.x+1-dataWindow.min.x) + tileWidth -1) / tileWidth;

    Int64 tilesY = (static_cast<Int64>(dataWindow.max.y+1-dataWindow.min.y) + tileHeight -1) / tileHeight;


    Int64 chunkCount = tilesX*tilesY;
    if ( chunkCount > gLargeChunkTableSize)
    {

        if (chunkCount > gLargeChunkTableSize)
        {
            Int64 pos = _streamData->is->tellg();
            _streamData->is->seekg(pos + (chunkCount-1)*sizeof(Int64));
            Int64 temp;
            OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*_streamData->is, temp);
            _streamData->is->seekg(pos);

        }
    }

}