    TEST(BSONValidateFast, NestedObject) {
        BSONObj x = BSON( "a" << 1 << "b" << BSON("c" << 2 << "d" << BSONArrayBuilder().obj() << "e" << BSON_ARRAY("1" << 2 << 3)));
        ASSERT_OK(validateBSON(x.objdata(), x.objsize()));
        ASSERT_NOT_OK(validateBSON(x.objdata(), x.objsize() / 2));
    }