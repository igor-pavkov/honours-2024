    Point& SGeometry::operator[](size_t index)
    {
        for(int order = 0; order < touple_order; order++)
        {
            Point& pt = *(this->pt_buffer);
            double data;
            size_t real_ind = index;

            // Read a single coord
            int err = nc_get_var1_double(ncid, nodec_varIds[order], &real_ind, &data);

            if(err != NC_NOERR)
            {
                throw SG_Exception_BadPoint();
            }

            pt[order] = data;
        }    

        return *(this->pt_buffer);
    }