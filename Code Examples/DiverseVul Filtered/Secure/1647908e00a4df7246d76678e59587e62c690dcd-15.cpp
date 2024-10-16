            Header(const ByteOrder& order, int v, int size, uint64_t offset):
                byteOrder_(order),
                version_(v),
                data_size_(size),
                dir_offset_(offset)
            {

            }