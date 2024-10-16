    TEST(BSONValidateFast, Simple3 ) {
        BSONObjBuilder b;
        char buf[64];
        for ( int i=1; i<=JSTypeMax; i++ ) {
            sprintf( buf, "foo%d", i );
            b.appendMinForType( buf, i );
            sprintf( buf, "bar%d", i );
            b.appendMaxForType( buf, i );
        }
        BSONObj x = b.obj();
        ASSERT_OK( validateBSON( x.objdata(), x.objsize() ) );
    }