    TEST(BSONValidate, MuckingData1) {

        BSONObj theObject;

        {
            BSONObjBuilder b;
            b.append( "name" , "eliot was here" );
            b.append( "yippee" , "asd" );
            BSONArrayBuilder a( b.subarrayStart( "arr" ) );
            for ( int i=0; i<100; i++ ) {
                a.append( BSON( "x" << i << "who" << "me" << "asd" << "asd" ) );
            }
            a.done();
            b.done();

            theObject = b.obj();
        }

        int numValid = 0;
        int numToRun = 1000;
        long long jsonSize = 0;

        for ( int i=4; i<theObject.objsize()-1; i++ ) {
            BSONObj mine = theObject.copy();

            char* data = const_cast<char*>(mine.objdata());

            data[ i ] = 200;

            numToRun++;
            if ( mine.valid() ) {
                numValid++;
                jsonSize += mine.jsonString().size();
                ASSERT_OK( validateBSON( mine.objdata(), mine.objsize() ) );
            }
            else {
                ASSERT_NOT_OK( validateBSON( mine.objdata(), mine.objsize() ) );
            }

        }

        log() << "MuckingData1: didn't crash valid/total: " << numValid << "/" << numToRun
              << " (want few valid ones) "
              << " jsonSize: " << jsonSize << endl;
    }