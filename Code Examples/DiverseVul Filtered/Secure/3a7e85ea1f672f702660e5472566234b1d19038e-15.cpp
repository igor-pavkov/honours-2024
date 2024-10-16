            void setIsCodeWithScope(bool isCodeWithScope) {
                if (isCodeWithScope) {
                    _startPosition |= 1 << 31;
                }
                else {
                    _startPosition &= ~(1 << 31);
                }
            }