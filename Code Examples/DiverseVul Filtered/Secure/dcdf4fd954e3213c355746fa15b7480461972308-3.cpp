    int find(const ByteVectorMirror &pattern, uint offset = 0, int byteAlign = 1) const
    {
      ByteVectorMirror v(*this);

      if(offset > 0) {
        offset = size() - offset - pattern.size();
        if(offset >= size())
          offset = 0;
      }

      const int pos = vectorFind<ByteVectorMirror>(v, pattern, offset, byteAlign);

      // If the offset is zero then we need to adjust the location in the search
      // to be appropriately reversed.  If not we need to account for the fact
      // that the recursive call (called from the above line) has already ajusted
      // for this but that the normal templatized find above will add the offset
      // to the returned value.
      //
      // This is a little confusing at first if you don't first stop to think
      // through the logic involved in the forward search.

      if(pos == -1)
        return -1;

      return size() - pos - pattern.size();
    }