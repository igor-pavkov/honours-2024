    void* inPlaceSerialize_LineString(SGeometry * ge, int node_count, size_t seek_begin, void * serializeBegin)
    {
        uint8_t order = PLATFORM_HEADER;
        uint32_t t = ge->get_axisCount() == 2 ? wkbLineString:
                     ge->get_axisCount() == 3 ? wkbLineString25D: wkbNone;

        if(t == wkbNone) throw SG_Exception_BadFeature();
        uint32_t nc = (uint32_t) node_count;
        
        serializeBegin = memcpy_jump(serializeBegin, &order, 1);
        serializeBegin = memcpy_jump(serializeBegin, &t, 4);
        serializeBegin = memcpy_jump(serializeBegin, &nc, 4);

        // Now serialize points
        for(int ind = 0; ind < node_count; ind++)
        {
            Point & p = (*ge)[seek_begin + ind];
            double x = p[0];
            double y = p[1];
            serializeBegin = memcpy_jump(serializeBegin, &x, 8);
            serializeBegin = memcpy_jump(serializeBegin, &y, 8);    
            
            if(ge->get_axisCount() >= 3)
            {
                double z = p[2];
                serializeBegin = memcpy_jump(serializeBegin, &z, 8);
            }
        }

        return serializeBegin;
    }