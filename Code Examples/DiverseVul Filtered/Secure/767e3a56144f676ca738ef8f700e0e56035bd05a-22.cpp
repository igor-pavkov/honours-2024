    int scanForGeometryContainers(int ncid, std::vector<int> & r_ids)
    {
        int nvars;
        if(nc_inq_nvars(ncid, &nvars) != NC_NOERR)
        {
            return -1;
        }

        r_ids.clear();

        // For each variable check for geometry attribute
        // If has geometry attribute, then check the associated variable ID

        for(int itr = 0; itr < nvars; itr++)
        {
            char c[NC_MAX_CHAR];
            memset(c, 0, NC_MAX_CHAR);
            if(nc_get_att_text(ncid, itr, CF_SG_GEOMETRY, c) != NC_NOERR)
            {
                continue;
            }

            int varID;
            if(nc_inq_varid(ncid, c, &varID) != NC_NOERR)
            {
                continue;
            }

            // Now have variable ID. See if vector contains it, and if not
            // insert
            bool contains = false;
            for(size_t itr_1 = 0; itr_1 < r_ids.size(); itr_1++)
            {
                if(r_ids[itr_1] == varID) contains = true;    
            }

            if(!contains)
            {
                r_ids.push_back(varID);
            }
        }    

        return 0 ;
    }