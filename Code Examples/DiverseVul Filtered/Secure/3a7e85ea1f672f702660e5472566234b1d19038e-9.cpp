            void setStartPosition(int pos) {
                _startPosition = (_startPosition & (1 << 31)) | (pos & ~(1 << 31));
            }