    void* inPlaceSerialize_PolygonExtOnly(SGeometry * ge, int node_count, size_t seek_begin, void * serializeBegin)
    {    
        int8_t header = PLATFORM_HEADER;
        uint32_t t = ge->get_axisCount() == 2 ? wkbPolygon:
                     ge->get_axisCount() == 3 ? wkbPolygon25D: wkbNone;

        if(t == wkbNone) throw SG_Exception_BadFeature();
        int32_t rc = 1;
                
        void * writer = serializeBegin;
        writer = memcpy_jump(writer, &header, 1);
        writer = memcpy_jump(writer, &t, 4);
        writer = memcpy_jump(writer, &rc, 4);
                        
        int32_t nc = (int32_t)node_count;
        writer = memcpy_jump(writer, &node_count, 4);

        for(int pind = 0; pind < nc; pind++)
        {
            Point & pt= (*ge)[seek_begin + pind];
            double x = pt[0]; double y = pt[1];
            writer = memcpy_jump(writer, &x, 8);
            writer = memcpy_jump(writer, &y, 8);
            
            if(ge->get_axisCount() >= 3)
            {
                double z = pt[2];
                writer = memcpy_jump(writer, &z, 8);
            }
        }

        return writer;
    }