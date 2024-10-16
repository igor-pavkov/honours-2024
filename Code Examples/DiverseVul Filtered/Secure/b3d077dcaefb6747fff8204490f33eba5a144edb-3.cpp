    uint16_t RotationMap::orientation(int32_t degrees)
    {
        uint16_t o = 1;
        for (int i = 0; omList_[i].orientation != 0; ++i) {
            if (omList_[i].degrees == degrees) {
                o = omList_[i].orientation;
                break;
            }
        }
        return o;
    }