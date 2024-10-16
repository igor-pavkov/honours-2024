    void* inPlaceSerialize_Polygon(SGeometry * ge, std::vector<int>& pnc, int ring_count, size_t seek_begin, void * serializeBegin)
    {
            
        int8_t header = PLATFORM_HEADER;
        uint32_t t = ge->get_axisCount() == 2 ? wkbPolygon:
                     ge->get_axisCount() == 3 ? wkbPolygon25D: wkbNone;

        if(t == wkbNone) throw SG_Exception_BadFeature();
        int32_t rc = static_cast<int32_t>(ring_count);
                
        void * writer = serializeBegin;
        writer = memcpy_jump(writer, &header, 1);
        writer = memcpy_jump(writer, &t, 4);
        writer = memcpy_jump(writer, &rc, 4);
        int cmoffset = 0;
                        
        for(int ring_c = 0; ring_c < ring_count; ring_c++)
        {
            int32_t node_count = pnc[ring_c];
            writer = memcpy_jump(writer, &node_count, 4);

            int pind = 0;
            for(pind = 0; pind < pnc[ring_c]; pind++)
            {
                Point & pt= (*ge)[seek_begin + cmoffset + pind];
                double x = pt[0]; double y = pt[1];
                writer = memcpy_jump(writer, &x, 8);
                writer = memcpy_jump(writer, &y, 8);
            
                if(ge->get_axisCount() >= 3)
                {
                     double z = pt[2];
                     writer = memcpy_jump(writer, &z, 8);
                }
            }

            cmoffset += pind;
        }

        return writer;
    }