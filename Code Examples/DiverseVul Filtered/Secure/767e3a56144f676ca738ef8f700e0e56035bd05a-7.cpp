    void SGeometry_PropertyScanner::open(int container_id)
    {
        // First check for container_id, if variable doesn't exist error out
        if(nc_inq_var(this->nc, container_id, nullptr, nullptr, nullptr, nullptr, nullptr) != NC_NOERR)
        {
            return;    // change to exception
        }

        // Now exists, see what variables refer to this one
        // First get name of this container
        char contname[NC_MAX_NAME + 1];
        memset(contname, 0, NC_MAX_NAME + 1);
        if(nc_inq_varname(this->nc, container_id, contname) != NC_NOERR)
        {
            return;
        }

        // Then scan throughout the netcdfDataset if those variables geometry_container
        // atrribute matches the container
        int varCount = 0;
        if(nc_inq_nvars(this->nc, &varCount) != NC_NOERR)
        {
            return;
        }

        for(int curr = 0; curr < varCount; curr++)
        {
            size_t contname2_len = 0;
            
            // First find container length, and make buf that size in chars
            if(nc_inq_attlen(this->nc, curr, CF_SG_GEOMETRY, &contname2_len) != NC_NOERR)
            {
                // not a geometry variable, continue
                continue;
            }

            // Also if present but empty, go on
            if(contname2_len == 0) continue;

            // Otherwise, geometry: see what container it has
            char buf[NC_MAX_CHAR + 1];
            memset(buf, 0, NC_MAX_CHAR + 1);

            if(nc_get_att_text(this->nc, curr, CF_SG_GEOMETRY, buf)!= NC_NOERR)
            {
                continue;
            }

            // If matches, then establish a reference by placing this variable's data in both vectors
            if(!strcmp(contname, buf))
            {
                char property_name[NC_MAX_NAME];
                nc_inq_varname(this->nc, curr, property_name);
                
                std::string n(property_name);
                v_ids.push_back(curr);
                v_headers.push_back(n);
            }
        }
    }