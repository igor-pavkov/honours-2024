    TEST(BSONValidateFast, Simple2 ) {
        char buf[64];
        for ( int i=1; i<=JSTypeMax; i++ ) {
            BSONObjBuilder b;
            sprintf( buf, "foo%d", i );
            b.appendMinForType( buf, i );
            sprintf( buf, "bar%d", i );
            b.appendMaxForType( buf, i );
            BSONObj x = b.obj();
            ASSERT_OK( validateBSON( x.objdata(), x.objsize() ) );
        }
    }