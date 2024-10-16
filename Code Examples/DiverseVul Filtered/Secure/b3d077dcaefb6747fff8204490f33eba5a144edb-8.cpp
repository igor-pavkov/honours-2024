    void CiffDirectory::doDecode(Image& image, ByteOrder byteOrder) const
    {
        Components::const_iterator b = components_.begin();
        Components::const_iterator e = components_.end();
        for (Components::const_iterator i = b; i != e; ++i) {
            (*i)->decode(image, byteOrder);
        }
    } // CiffDirectory::doDecode