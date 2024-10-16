    double getCFVersion(int ncid)
    {
        double ver = -1.0;
        std::string attrVal;

        // Fetch the CF attribute
        if(attrf(ncid, NC_GLOBAL, NCDF_CONVENTIONS, attrVal) == "")
        {
            return ver;
        }

        if(sscanf(attrVal.c_str(), "CF-%lf", &ver) != 1)
        {
            return -1.0;
        }

        return ver;
    }