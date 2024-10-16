    size_t SGeometry::get_geometry_count()
    {
        if(type == POINT)
        {
            // If nodes global attribute is available, use that

            // Otherwise, don't fail- use dimension length of one of x

            if(this->nodec_varIds.size() < 1) return 0;

            // If more than one dim, then error. Otherwise inquire its length and return that
            int dims;
            if(nc_inq_varndims(this->ncid, nodec_varIds[0], &dims) != NC_NOERR) return 0;
            if(dims != 1) return 0;
            
            // Find which dimension is used for x
            int index;
            if(nc_inq_vardimid(this->ncid, nodec_varIds[0], &index) != NC_NOERR)
            {
                return 0;
            }

            // Finally find the length
            size_t len;
            if(nc_inq_dimlen(this->ncid, index, &len) != NC_NOERR)
            {
                return 0;
            }
            return len;    
        }

        else return this->node_counts.size();
    }