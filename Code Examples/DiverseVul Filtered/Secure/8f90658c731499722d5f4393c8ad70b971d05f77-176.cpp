      void dump_xml(Formatter *f) const {
        encode_xml("AccessControlTranslation", acl_translation, f);
        encode_xml("Account", account, f);
        encode_xml("Bucket", bucket, f);
        encode_xml("StorageClass", storage_class, f);
        encode_xml("Zone", zone_names, f);
      }