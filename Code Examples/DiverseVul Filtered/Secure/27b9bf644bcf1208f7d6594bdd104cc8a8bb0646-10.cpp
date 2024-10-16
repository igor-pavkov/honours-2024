GIntBig OGRKMLLayer::GetFeatureCount( int bForce )
{
    if( m_poFilterGeom != nullptr || m_poAttrQuery != nullptr )
        return OGRLayer::GetFeatureCount(bForce);

    KML *poKMLFile = poDS_->GetKMLFile();
    if( nullptr == poKMLFile )
        return 0;

    poKMLFile->selectLayer(nLayerNumber_);

    return poKMLFile->getNumFeatures();
}