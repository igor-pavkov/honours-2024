    Point& SGeometry::next_pt()
    {
        if(!this->has_next_pt())
        {
            throw SG_Exception_BadPoint();
        }

        // Fill pt
        // New pt now
        for(int order = 0; order < touple_order; order++)
        {
            Point& pt = *(this->pt_buffer);
            double data;
            size_t full_ind = bound_list[cur_geometry_ind] + current_vert_ind;

            // Read a single coord
            int err = nc_get_var1_double(ncid, nodec_varIds[order], &full_ind, &data);
            // To do: optimize through multiple reads at once, instead of one datum

            if(err != NC_NOERR)
            {
                throw SG_Exception_BadPoint();
            }

            pt[order] = data;
        }    
        
        this->current_vert_ind++;
        return *(this->pt_buffer);    
    }