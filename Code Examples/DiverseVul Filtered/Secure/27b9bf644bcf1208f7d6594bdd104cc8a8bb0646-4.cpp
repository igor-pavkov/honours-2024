OGRErr OGRKMLLayer::ICreateFeature( OGRFeature* poFeature )
{
    CPLAssert( nullptr != poFeature );
    CPLAssert( nullptr != poDS_ );

    if( !bWriter_ )
        return OGRERR_FAILURE;

    if( bClosedForWriting )
    {
        CPLError(
            CE_Failure, CPLE_NotSupported,
            "Interleaved feature adding to different layers is not supported");
        return OGRERR_FAILURE;
    }

    VSILFILE *fp = poDS_->GetOutputFP();
    CPLAssert( nullptr != fp );

    if( poDS_->GetLayerCount() == 1 && nWroteFeatureCount_ == 0 )
    {
        CPLString osRet = WriteSchema();
        if( !osRet.empty() )
            VSIFPrintfL( fp, "%s", osRet.c_str() );
        bSchemaWritten_ = true;

        VSIFPrintfL( fp, "<Folder><name>%s</name>\n", pszName_);
    }

    VSIFPrintfL( fp, "  <Placemark>\n" );

    if( poFeature->GetFID() == OGRNullFID )
        poFeature->SetFID( iNextKMLId_++ );

    // Find and write the name element
    if( nullptr != poDS_->GetNameField() )
    {
        for( int iField = 0;
             iField < poFeatureDefn_->GetFieldCount();
             iField++ )
        {
            OGRFieldDefn *poField = poFeatureDefn_->GetFieldDefn( iField );

            if( poFeature->IsFieldSetAndNotNull( iField )
                && EQUAL(poField->GetNameRef(), poDS_->GetNameField()) )
            {
                const char *pszRaw = poFeature->GetFieldAsString( iField );
                while( *pszRaw == ' ' )
                    pszRaw++;

                char *pszEscaped = OGRGetXML_UTF8_EscapedString( pszRaw );

                VSIFPrintfL( fp, "\t<name>%s</name>\n", pszEscaped);
                CPLFree( pszEscaped );
            }
        }
    }

    if( nullptr != poDS_->GetDescriptionField() )
    {
        for( int iField = 0;
             iField < poFeatureDefn_->GetFieldCount();
             iField++ )
        {
            OGRFieldDefn *poField = poFeatureDefn_->GetFieldDefn( iField );

            if( poFeature->IsFieldSetAndNotNull( iField )
                && EQUAL(poField->GetNameRef(), poDS_->GetDescriptionField()) )
            {
                const char *pszRaw = poFeature->GetFieldAsString( iField );
                while( *pszRaw == ' ' )
                    pszRaw++;

                char *pszEscaped = OGRGetXML_UTF8_EscapedString( pszRaw );

                VSIFPrintfL( fp, "\t<description>%s</description>\n",
                             pszEscaped);
                CPLFree( pszEscaped );
            }
        }
    }

    OGRwkbGeometryType eGeomType = wkbNone;
    if( poFeature->GetGeometryRef() != nullptr )
        eGeomType = wkbFlatten(poFeature->GetGeometryRef()->getGeometryType());

    if( wkbPolygon == eGeomType
        || wkbMultiPolygon == eGeomType
        || wkbLineString == eGeomType
        || wkbMultiLineString == eGeomType )
    {
        OGRStylePen *poPen = nullptr;
        OGRStyleMgr oSM;

        if( poFeature->GetStyleString() != nullptr )
        {
            oSM.InitFromFeature( poFeature );

            for( int i = 0; i < oSM.GetPartCount(); i++ )
            {
                OGRStyleTool *poTool = oSM.GetPart(i);
                if (poTool && poTool->GetType() == OGRSTCPen )
                {
                    poPen = (OGRStylePen*) poTool;
                    break;
                }
                delete poTool;
            }
        }

        VSIFPrintfL( fp, "\t<Style>");
        if( poPen != nullptr )
        {
            bool bHasWidth = false;
            GBool bDefault = FALSE;

            /* Require width to be returned in pixel */
            poPen->SetUnit(OGRSTUPixel);
            double fW = poPen->Width(bDefault);
            if( bDefault )
                fW = 1;
            else
                bHasWidth = true;
            const char* pszColor = poPen->Color(bDefault);
            const int nColorLen = static_cast<int>(CPLStrnlen(pszColor, 10));
            if( pszColor != nullptr &&
                pszColor[0] == '#' &&
                !bDefault && nColorLen >= 7)
            {
                char acColor[9] = {0};
                /* Order of KML color is aabbggrr, whereas OGR color is #rrggbb[aa] ! */
                if(nColorLen == 9)
                {
                    acColor[0] = pszColor[7]; /* A */
                    acColor[1] = pszColor[8];
                }
                else
                {
                    acColor[0] = 'F';
                    acColor[1] = 'F';
                }
                acColor[2] = pszColor[5]; /* B */
                acColor[3] = pszColor[6];
                acColor[4] = pszColor[3]; /* G */
                acColor[5] = pszColor[4];
                acColor[6] = pszColor[1]; /* R */
                acColor[7] = pszColor[2];
                VSIFPrintfL( fp, "<LineStyle><color>%s</color>", acColor);
                if (bHasWidth)
                    VSIFPrintfL( fp, "<width>%g</width>", fW);
                VSIFPrintfL( fp, "</LineStyle>");
            }
            else
            {
                VSIFPrintfL(
                    fp, "<LineStyle><color>ff0000ff</color></LineStyle>");
            }
        }
        else
        {
            VSIFPrintfL( fp, "<LineStyle><color>ff0000ff</color></LineStyle>");
        }
        delete poPen;
        // If we're dealing with a polygon, add a line style that will stand out
        // a bit.
        VSIFPrintfL( fp, "<PolyStyle><fill>0</fill></PolyStyle></Style>\n" );
    }

    bool bHasFoundOtherField = false;

    // Write all fields as SchemaData
    for( int iField = 0; iField < poFeatureDefn_->GetFieldCount(); iField++ )
    {
        OGRFieldDefn *poField = poFeatureDefn_->GetFieldDefn( iField );

        if( poFeature->IsFieldSetAndNotNull( iField ))
        {
            if (nullptr != poDS_->GetNameField() &&
                EQUAL(poField->GetNameRef(), poDS_->GetNameField()) )
                continue;

            if (nullptr != poDS_->GetDescriptionField() &&
                EQUAL(poField->GetNameRef(), poDS_->GetDescriptionField()) )
                continue;

            if( !bHasFoundOtherField )
            {
                VSIFPrintfL( fp, "\t<ExtendedData><SchemaData schemaUrl=\"#%s\">\n", pszName_ );
                bHasFoundOtherField = true;
            }
            const char *pszRaw = poFeature->GetFieldAsString( iField );

            while( *pszRaw == ' ' )
                pszRaw++;

            char *pszEscaped = nullptr;
            if (poFeatureDefn_->GetFieldDefn(iField)->GetType() == OFTReal)
            {
                pszEscaped = CPLStrdup( pszRaw );
            }
            else
            {
                pszEscaped = OGRGetXML_UTF8_EscapedString( pszRaw );
            }

            VSIFPrintfL( fp, "\t\t<SimpleData name=\"%s\">%s</SimpleData>\n",
                        poField->GetNameRef(), pszEscaped);

            CPLFree( pszEscaped );
        }
    }

    if( bHasFoundOtherField )
    {
        VSIFPrintfL( fp, "\t</SchemaData></ExtendedData>\n" );
    }

    // Write out Geometry - for now it isn't indented properly.
    if( poFeature->GetGeometryRef() != nullptr )
    {
        char* pszGeometry = nullptr;
        OGREnvelope sGeomBounds;
        OGRGeometry *poWGS84Geom = nullptr;

        if (nullptr != poCT_)
        {
            poWGS84Geom = poFeature->GetGeometryRef()->clone();
            poWGS84Geom->transform( poCT_ );
        }
        else
        {
            poWGS84Geom = poFeature->GetGeometryRef();
        }

        // TODO - porting
        // pszGeometry = poFeature->GetGeometryRef()->exportToKML();
        pszGeometry =
            OGR_G_ExportToKML( (OGRGeometryH)poWGS84Geom,
                               poDS_->GetAltitudeMode());
        if( pszGeometry != nullptr )
        {
            VSIFPrintfL( fp, "      %s\n", pszGeometry );
        }
        else
        {
            CPLError(CE_Failure, CPLE_AppDefined,
                     "Export of geometry to KML failed");
        }
        CPLFree( pszGeometry );

        poWGS84Geom->getEnvelope( &sGeomBounds );
        poDS_->GrowExtents( &sGeomBounds );

        if (nullptr != poCT_)
        {
            delete poWGS84Geom;
        }
    }

    VSIFPrintfL( fp, "  </Placemark>\n" );
    nWroteFeatureCount_++;
    return OGRERR_NONE;
}