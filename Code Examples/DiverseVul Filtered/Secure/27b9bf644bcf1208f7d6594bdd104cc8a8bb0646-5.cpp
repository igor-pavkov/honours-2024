OGRKMLLayer::~OGRKMLLayer()
{
    if( nullptr != poFeatureDefn_ )
        poFeatureDefn_->Release();

    if( nullptr != poSRS_ )
        poSRS_->Release();

    if( nullptr != poCT_ )
        delete poCT_;

    CPLFree( pszName_ );
}