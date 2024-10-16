int OGRKMLLayer::TestCapability( const char * pszCap )
{
    if( EQUAL(pszCap, OLCSequentialWrite) )
    {
        return bWriter_;
    }
    else if( EQUAL(pszCap, OLCCreateField) )
    {
        return bWriter_ && iNextKMLId_ == 0;
    }
    else if( EQUAL(pszCap,OLCFastFeatureCount) )
    {
//        if( poFClass == NULL
//            || m_poFilterGeom != NULL
//            || m_poAttrQuery != NULL )
            return FALSE;

//        return poFClass->GetFeatureCount() != -1;
    }

    else if (EQUAL(pszCap, OLCStringsAsUTF8))
        return TRUE;

    return FALSE;
}