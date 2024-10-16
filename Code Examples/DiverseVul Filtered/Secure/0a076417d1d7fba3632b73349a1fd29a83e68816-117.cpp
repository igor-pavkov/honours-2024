    BSONObj reverseSpec() {
        return BSON("$strcasecmp" << BSON_ARRAY(b() << a()));
    }