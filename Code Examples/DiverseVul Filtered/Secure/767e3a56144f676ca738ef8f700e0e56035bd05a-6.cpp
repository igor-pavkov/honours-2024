    SGeometry* getGeometryRef(int ncid, const char * varName )
    {
        int varId = 0;
        nc_inq_varid(ncid, varName, &varId);
        return new SGeometry(ncid, varId);
    }