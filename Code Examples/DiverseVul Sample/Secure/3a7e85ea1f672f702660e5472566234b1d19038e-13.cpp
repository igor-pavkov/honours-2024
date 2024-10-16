    TEST(BSONValidate, RandomData) {
        PseudoRandom r(17);

        int numValid = 0;
        int numToRun = 1000;
        long long jsonSize = 0;

        for ( int i=0; i<numToRun; i++ ) {
            int size = 1234;

            char* x = new char[size];
            int* xx = reinterpret_cast<int*>(x);
            xx[0] = size;

            for ( int i=4; i<size; i++ ) {
                x[i] = r.nextInt32( 255 );
            }

            x[size-1] = 0;

            BSONObj o( x );

            ASSERT_EQUALS( size, o.objsize() );

            if ( o.valid() ) {
                numValid++;
                jsonSize += o.jsonString().size();
                ASSERT_OK( validateBSON( o.objdata(), o.objsize() ) );
            }
            else {
                ASSERT_NOT_OK( validateBSON( o.objdata(), o.objsize() ) );
            }

            delete[] x;
        }

        log() << "RandomData: didn't crash valid/total: " << numValid << "/" << numToRun
              << " (want few valid ones)"
              << " jsonSize: " << jsonSize << endl;
    }