    void CiffDirectory::doAdd(AutoPtr component)
    {
        components_.push_back(component.release());
    } // CiffDirectory::doAdd