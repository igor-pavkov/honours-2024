    BSONObj spec() {
        return BSON("$strcasecmp" << BSON_ARRAY(a() << b()));
    }