MultiPartInputFile::Data::readChunkOffsetTables(bool reconstructChunkOffsetTable)
{
    bool brokenPartsExist = false;

    for (size_t i = 0; i < parts.size(); i++)
    {
        int chunkOffsetTableSize = getChunkOffsetTableSize(parts[i]->header);

        //
        // avoid allocating excessive memory.
        // If the chunktablesize claims to be large,
        // check the file is big enough to contain the table before allocating memory.
        // Attempt to read the last entry in the table. Either the seekg() or the read()
        // call will throw an exception if the file is too small to contain the table
        //
        if (chunkOffsetTableSize > gLargeChunkTableSize)
        {
            Int64 pos = is->tellg();
            is->seekg(pos + (chunkOffsetTableSize-1)*sizeof(Int64));
            Int64 temp;
            OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*is, temp);
            is->seekg(pos);

        }

        parts[i]->chunkOffsets.resize(chunkOffsetTableSize);



        for (int j = 0; j < chunkOffsetTableSize; j++)
            OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*is, parts[i]->chunkOffsets[j]);

        //
        // Check chunk offsets, reconstruct if broken.
        // At first we assume the table is complete.
        //
        parts[i]->completed = true;
        for (int j = 0; j < chunkOffsetTableSize; j++)
        {
            if (parts[i]->chunkOffsets[j] <= 0)
            {
                brokenPartsExist = true;
                parts[i]->completed = false;
                break;
            }
        }
    }

    if (brokenPartsExist && reconstructChunkOffsetTable)
        chunkOffsetReconstruction(*is, parts);
}