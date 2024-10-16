    void CrwMap::decode(const CiffComponent& ciffComponent,
                        Image&               image,
                        ByteOrder            byteOrder)
    {
        const CrwMapping* cmi = crwMapping(ciffComponent.dir(),
                                           ciffComponent.tagId());
        if (cmi && cmi->toExif_) {
            cmi->toExif_(ciffComponent, cmi, image, byteOrder);
        }
    } // CrwMap::decode