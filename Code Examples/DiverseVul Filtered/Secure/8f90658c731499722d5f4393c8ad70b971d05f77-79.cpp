      void decode_xml(XMLObj *obj) {
        RGWXMLDecoder::decode_xml("AccessControlTranslation", acl_translation, obj);
        RGWXMLDecoder::decode_xml("Account", account, obj);
        if (account && account->empty()) {
          account.reset();
        }
        RGWXMLDecoder::decode_xml("Bucket", bucket, obj);
        RGWXMLDecoder::decode_xml("StorageClass", storage_class, obj);
        if (storage_class && storage_class->empty()) {
          storage_class.reset();
        }
        RGWXMLDecoder::decode_xml("Zone", zone_names, obj); /* rgw extension */
      }