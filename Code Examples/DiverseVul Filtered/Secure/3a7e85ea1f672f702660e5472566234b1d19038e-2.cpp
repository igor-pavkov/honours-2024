    TEST( BSONValidate, Fuzz ) {
        int64_t seed = time( 0 );
        log() << "BSONValidate Fuzz random seed: " << seed << endl;
        PseudoRandom randomSource( seed );

        BSONObj original = BSON( "one" << 3 <<
                                 "two" << 5 <<
                                 "three" << BSONObj() <<
                                 "four" << BSON( "five" << BSON( "six" << 11 ) ) <<
                                 "seven" << BSON_ARRAY( "a" << "bb" << "ccc" << 5 ) <<
                                 "eight" << BSONDBRef( "rrr", OID( "01234567890123456789aaaa" ) ) <<
                                 "_id" << OID( "deadbeefdeadbeefdeadbeef" ) <<
                                 "nine" << BSONBinData( "\x69\xb7", 2, BinDataGeneral ) <<
                                 "ten" << Date_t( 44 ) <<
                                 "eleven" << BSONRegEx( "foooooo", "i" ) );
        
        int32_t fuzzFrequencies[] = { 2, 10, 20, 100, 1000 };
        for( size_t i = 0; i < sizeof( fuzzFrequencies ) / sizeof( int32_t ); ++i ) {
            int32_t fuzzFrequency = fuzzFrequencies[ i ];

            // Copy the 'original' BSONObj to 'buffer'.
            scoped_array<char> buffer( new char[ original.objsize() ] );
            memcpy( buffer.get(), original.objdata(), original.objsize() );

            // Randomly flip bits in 'buffer', with probability determined by 'fuzzFrequency'. The
            // first four bytes, representing the size of the object, are excluded from bit
            // flipping.
            for( int32_t byteIdx = 4; byteIdx < original.objsize(); ++byteIdx ) {
                for( int32_t bitIdx = 0; bitIdx < 8; ++bitIdx ) {
                    if ( randomSource.nextInt32( fuzzFrequency ) == 0 ) {
                        reinterpret_cast<unsigned char&>( buffer[ byteIdx ] ) ^= ( 1U << bitIdx );
                    }
                }
            }
            BSONObj fuzzed( buffer.get() );

            // Check that the two validation implementations agree (and neither crashes).
            ASSERT_EQUALS( fuzzed.valid(),
                           validateBSON( fuzzed.objdata(), fuzzed.objsize() ).isOK() );
        }
    }