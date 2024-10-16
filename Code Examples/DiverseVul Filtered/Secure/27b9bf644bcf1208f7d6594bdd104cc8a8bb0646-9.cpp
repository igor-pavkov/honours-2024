OGRErr OGRKMLLayer::CreateField( OGRFieldDefn *poField,
                                 CPL_UNUSED int bApproxOK )
{
    if( !bWriter_ || iNextKMLId_ != 0 )
        return OGRERR_FAILURE;

    OGRFieldDefn oCleanCopy( poField );
    poFeatureDefn_->AddFieldDefn( &oCleanCopy );

    return OGRERR_NONE;
}