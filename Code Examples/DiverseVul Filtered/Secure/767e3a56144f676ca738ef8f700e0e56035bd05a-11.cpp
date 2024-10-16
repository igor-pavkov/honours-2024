    void* inPlaceSerialize_Point(SGeometry * ge, size_t seek_pos, void * serializeBegin)
    {
        uint8_t order = 1;
        uint32_t t = ge->get_axisCount() == 2 ? wkbPoint:
                     ge->get_axisCount() == 3 ? wkbPoint25D: wkbNone;

        if(t == wkbNone) throw SG_Exception_BadFeature();

        serializeBegin = memcpy_jump(serializeBegin, &order, 1);
        serializeBegin = memcpy_jump(serializeBegin, &t, 4);

        // Now get point data;
        Point & p = (*ge)[seek_pos];
        double x = p[0];
        double y = p[1];
        serializeBegin = memcpy_jump(serializeBegin, &x, 8);
        serializeBegin = memcpy_jump(serializeBegin, &y, 8);

        if(ge->get_axisCount() >= 3)
        {
            double z = p[2];
            serializeBegin = memcpy_jump(serializeBegin, &z, 8);
        }

        return serializeBegin;
    }