  String getCompressionMethod() {
    switch (m_zipStat.comp_method) {
      case 0:
        return "stored";
      case 1:
        return "shrunk";
      case 2:
      case 3:
      case 4:
      case 5:
        return "reduced";
      case 6:
        return "imploded";
      case 7:
        return "tokenized";
      case 8:
        return "deflated";
      case 9:
        return "deflatedX";
      case 10:
        return "implodedX";
      default:
        return false;
    }
  }