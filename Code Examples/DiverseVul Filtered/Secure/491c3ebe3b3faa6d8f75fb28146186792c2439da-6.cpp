    static bool zlibToCompressed(const byte* bytes, long length, DataBuf& result)
    {
        uLongf compressedLen = length;  // just a starting point
        int zlibResult;

        do {
            result.alloc(compressedLen);
            zlibResult = compress((Bytef*)result.pData_, &compressedLen, bytes, length);
            if (zlibResult == Z_BUF_ERROR) {
                // the compressedArray needs to be larger
                result.free();
                compressedLen *= 2;
            } else {
                result.free();
                result.alloc(compressedLen);
                zlibResult = compress((Bytef*)result.pData_, &compressedLen, bytes, length);
            }
        } while (zlibResult == Z_BUF_ERROR);

        return zlibResult == Z_OK;
    }