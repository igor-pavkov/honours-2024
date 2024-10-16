OGRFeature *OGRKMLLayer::GetNextFeature()
{
#ifndef HAVE_EXPAT
    return nullptr;
#else
    /* -------------------------------------------------------------------- */
    /*      Loop till we find a feature matching our criteria.              */
    /* -------------------------------------------------------------------- */
    KML *poKMLFile = poDS_->GetKMLFile();
    if( poKMLFile == nullptr )
        return nullptr;

    poKMLFile->selectLayer(nLayerNumber_);

    while( true )
    {
        Feature *poFeatureKML =
            poKMLFile->getFeature(iNextKMLId_++, nLastAsked, nLastCount);

        if( poFeatureKML == nullptr )
            return nullptr;

        OGRFeature *poFeature = new OGRFeature( poFeatureDefn_ );

        if( poFeatureKML->poGeom )
        {
            poFeature->SetGeometryDirectly(poFeatureKML->poGeom);
            poFeatureKML->poGeom = nullptr;
        }

        // Add fields.
        poFeature->SetField( poFeatureDefn_->GetFieldIndex("Name"),
                             poFeatureKML->sName.c_str() );
        poFeature->SetField( poFeatureDefn_->GetFieldIndex("Description"),
                             poFeatureKML->sDescription.c_str() );
        poFeature->SetFID( iNextKMLId_ - 1 );

        // Clean up.
        delete poFeatureKML;

        if( poFeature->GetGeometryRef() != nullptr && poSRS_ != nullptr )
        {
            poFeature->GetGeometryRef()->assignSpatialReference( poSRS_ );
        }

        // Check spatial/attribute filters.
        if( (m_poFilterGeom == nullptr ||
             FilterGeometry( poFeature->GetGeometryRef() ) ) &&
            (m_poAttrQuery == nullptr || m_poAttrQuery->Evaluate( poFeature )) )
        {
            return poFeature;
        }

        delete poFeature;
    }

#endif /* HAVE_EXPAT */
}