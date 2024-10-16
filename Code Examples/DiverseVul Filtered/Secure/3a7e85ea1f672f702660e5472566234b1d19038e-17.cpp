    TEST( BSONValidateFast, Empty ) {
        BSONObj x;
        ASSERT_OK( validateBSON( x.objdata(), x.objsize() ) );
    }