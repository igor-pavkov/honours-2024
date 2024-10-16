    CiffHeader::~CiffHeader()
    {
        delete   pRootDir_;
        delete[] pPadding_;
    }