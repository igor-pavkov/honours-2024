    static bool zlibToDataBuf(const byte* bytes,long length, DataBuf& result)
    {
        uLongf uncompressedLen = length * 2; // just a starting point
        int    zlibResult;

        do {
            result.alloc(uncompressedLen);
            zlibResult = uncompress((Bytef*)result.pData_,&uncompressedLen,bytes,length);
            // if result buffer is large than necessary, redo to fit perfectly.
            if (zlibResult == Z_OK && (long) uncompressedLen < result.size_ ) {
                result.free();

                result.alloc(uncompressedLen);
                zlibResult = uncompress((Bytef*)result.pData_,&uncompressedLen,bytes,length);
            }
            if (zlibResult == Z_BUF_ERROR) {
                // the uncompressed buffer needs to be larger
                result.free();

                // Sanity - never bigger than 16mb
                if  (uncompressedLen > 16*1024*1024) zlibResult = Z_DATA_ERROR;
                else uncompressedLen *= 2;
            }
        } while (zlibResult == Z_BUF_ERROR);

        return zlibResult == Z_OK ;
    }