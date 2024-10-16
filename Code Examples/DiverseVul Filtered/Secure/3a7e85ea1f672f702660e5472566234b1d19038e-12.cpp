    TEST( BSONValidateFast, RegEx ) {
        BSONObjBuilder b;
        b.appendRegex( "foo", "i" );
        BSONObj x = b.obj();
        ASSERT_OK( validateBSON( x.objdata(), x.objsize() ) );
    }