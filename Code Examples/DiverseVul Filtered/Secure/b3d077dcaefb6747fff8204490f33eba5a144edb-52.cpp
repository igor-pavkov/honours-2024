    int32_t RotationMap::degrees(uint16_t orientation)
    {
        int32_t d = 0;
        for (int i = 0; omList_[i].orientation != 0; ++i) {
            if (omList_[i].orientation == orientation) {
                d = omList_[i].degrees;
                break;
            }
        }
        return d;
    }