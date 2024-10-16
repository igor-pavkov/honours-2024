    geom_t getGeometryType(int ncid, int varid)
    {
        geom_t ret = UNSUPPORTED;
        std::string gt_name_s;
        const char * gt_name= attrf(ncid, varid, CF_SG_GEOMETRY_TYPE, gt_name_s).c_str();

        if(gt_name == nullptr)
        {
            return NONE;
        }
        
        // Points    
        if(!strcmp(gt_name, CF_SG_TYPE_POINT))
        {
            // Node Count not present? Assume that it is a multipoint.
            if(nc_inq_att(ncid, varid, CF_SG_NODE_COUNT, nullptr, nullptr) == NC_ENOTATT)
            {
                ret = POINT;    
            }
            else ret = MULTIPOINT;
        }

        // Lines
        else if(!strcmp(gt_name, CF_SG_TYPE_LINE))
        {
            // Part Node Count present? Assume multiline
            if(nc_inq_att(ncid, varid, CF_SG_PART_NODE_COUNT, nullptr, nullptr) == NC_ENOTATT)
            {
                ret = LINE;
            }
            else ret = MULTILINE;
        }

        // Polygons
        else if(!strcmp(gt_name, CF_SG_TYPE_POLY))
        {
            /* Polygons versus MultiPolygons, slightly ambiguous
             * Part Node Count & no Interior Ring - MultiPolygon
             * no Part Node Count & no Interior Ring - Polygon
             * Part Node Count & Interior Ring - assume that it is a MultiPolygon
             */
            int pnc_present = nc_inq_att(ncid, varid, CF_SG_PART_NODE_COUNT, nullptr, nullptr);
            int ir_present = nc_inq_att(ncid, varid, CF_SG_INTERIOR_RING, nullptr, nullptr);

            if(pnc_present == NC_ENOTATT && ir_present == NC_ENOTATT)
            {
                ret = POLYGON;
            }
            else ret = MULTIPOLYGON;
        }

        return ret;
    }