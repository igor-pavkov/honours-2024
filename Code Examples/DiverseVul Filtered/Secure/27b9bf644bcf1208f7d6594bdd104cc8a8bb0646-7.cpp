CPLString OGRKMLLayer::WriteSchema()
{
    if( bSchemaWritten_ )
        return "";

    CPLString osRet;

    OGRFeatureDefn *featureDefinition = GetLayerDefn();
    for( int j = 0; j < featureDefinition->GetFieldCount(); j++ )
    {
        OGRFieldDefn *fieldDefinition = featureDefinition->GetFieldDefn(j);

        if (nullptr != poDS_->GetNameField() &&
            EQUAL(fieldDefinition->GetNameRef(), poDS_->GetNameField()) )
            continue;

        if (nullptr != poDS_->GetDescriptionField() &&
            EQUAL(fieldDefinition->GetNameRef(), poDS_->GetDescriptionField()) )
            continue;

        if( osRet.empty() )
        {
            osRet += CPLSPrintf( "<Schema name=\"%s\" id=\"%s\">\n",
                                 pszName_, pszName_ );
        }

        const char* pszKMLType = nullptr;
        const char* pszKMLEltName = nullptr;
        // Match the OGR type to the GDAL type.
        switch (fieldDefinition->GetType())
        {
          case OFTInteger:
            pszKMLType = "int";
            pszKMLEltName = "SimpleField";
            break;
          case OFTIntegerList:
            pszKMLType = "int";
            pszKMLEltName = "SimpleArrayField";
            break;
          case OFTReal:
            pszKMLType = "float";
            pszKMLEltName = "SimpleField";
            break;
          case OFTRealList:
            pszKMLType = "float";
            pszKMLEltName = "SimpleArrayField";
            break;
          case OFTString:
            pszKMLType = "string";
            pszKMLEltName = "SimpleField";
            break;
          case OFTStringList:
            pszKMLType = "string";
            pszKMLEltName = "SimpleArrayField";
            break;
            //TODO: KML doesn't handle these data types yet...
          case OFTDate:
          case OFTTime:
          case OFTDateTime:
            pszKMLType = "string";
            pszKMLEltName = "SimpleField";
            break;

          default:
            pszKMLType = "string";
            pszKMLEltName = "SimpleField";
            break;
        }
        osRet += CPLSPrintf( "\t<%s name=\"%s\" type=\"%s\"></%s>\n",
                    pszKMLEltName, fieldDefinition->GetNameRef() ,pszKMLType, pszKMLEltName );
    }

    if( !osRet.empty() )
        osRet += CPLSPrintf( "%s", "</Schema>\n" );

    return osRet;
}