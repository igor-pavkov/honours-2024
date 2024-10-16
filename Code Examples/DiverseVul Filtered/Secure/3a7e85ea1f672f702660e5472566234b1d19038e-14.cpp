    TEST(BSONValidateFast, Simple0 ) {
        BSONObj x;
        ASSERT_OK( validateBSON( x.objdata(), x.objsize() ) );

        x = BSON( "foo" << 17 << "bar" << "eliot" );
        ASSERT_OK( validateBSON( x.objdata(), x.objsize() ) );

    }