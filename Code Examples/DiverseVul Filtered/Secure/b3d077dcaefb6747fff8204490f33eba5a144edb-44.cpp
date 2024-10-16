    CiffComponent::~CiffComponent()
    {
        if (isAllocated_) delete[] pData_;
    }