    BSONObj spec() {
        return BSON("$substrBytes" << BSON_ARRAY(str() << offset() << length()));
    }