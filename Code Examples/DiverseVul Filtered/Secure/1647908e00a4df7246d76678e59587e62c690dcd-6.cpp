            Format format() const
            {
                assert(isValid());
                return version_ == 0x2A? StandardTiff: BigTiff;
            }