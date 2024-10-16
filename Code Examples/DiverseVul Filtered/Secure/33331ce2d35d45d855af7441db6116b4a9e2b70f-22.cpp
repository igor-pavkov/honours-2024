    void Release() {
        if (--m_ReferenceCount == 0) {
            delete this;
        }
    }