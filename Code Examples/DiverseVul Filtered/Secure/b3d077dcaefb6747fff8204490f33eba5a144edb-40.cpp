    void CrwMap::encode(CiffHeader* pHead, const Image& image)
    {
        for (const CrwMapping* cmi = crwMapping_; cmi->ifdId_ != ifdIdNotSet; ++cmi) {
            if (cmi->fromExif_ != 0) {
                cmi->fromExif_(image, cmi, pHead);
            }
        }
    } // CrwMap::encode