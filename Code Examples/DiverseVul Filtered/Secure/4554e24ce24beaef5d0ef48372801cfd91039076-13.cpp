void CLASS process_Sony_0x9400(uchar *buf, ushort len, unsigned id)
{

  uchar s[4];
  int c;
  short bufx = buf[0];

  if (((bufx == 0x23) || (bufx == 0x24) || (bufx == 0x26)) && (len >= 0x1f))
  { // 0x9400 'c' version

    if ((id == 358) || (id == 362) || (id == 365))
    {
      imgdata.makernotes.sony.ShotNumberSincePowerUp = SonySubstitution[buf[0x0a]];
    }
    else
    {
      FORC4 s[c] = SonySubstitution[buf[0x0a + c]];
      imgdata.makernotes.sony.ShotNumberSincePowerUp = sget4(s);
    }

    imgdata.makernotes.sony.Sony0x9400_version = 0xc;

    imgdata.makernotes.sony.Sony0x9400_ReleaseMode2 = SonySubstitution[buf[0x09]];

    FORC4 s[c] = SonySubstitution[buf[0x12 + c]];
    imgdata.makernotes.sony.Sony0x9400_SequenceImageNumber = sget4(s);

    imgdata.makernotes.sony.Sony0x9400_SequenceLength1 = SonySubstitution[buf[0x16]]; // shots

    FORC4 s[c] = SonySubstitution[buf[0x1a + c]];
    imgdata.makernotes.sony.Sony0x9400_SequenceFileNumber = sget4(s);

    imgdata.makernotes.sony.Sony0x9400_SequenceLength2 = SonySubstitution[buf[0x1e]]; // files
  }

  else if ((bufx == 0x0c) && (len >= 0x1f))
  { // 0x9400 'b' version
    imgdata.makernotes.sony.Sony0x9400_version = 0xb;

    FORC4 s[c] = SonySubstitution[buf[0x08 + c]];
    imgdata.makernotes.sony.Sony0x9400_SequenceImageNumber = sget4(s);

    FORC4 s[c] = SonySubstitution[buf[0x0c + c]];
    imgdata.makernotes.sony.Sony0x9400_SequenceFileNumber = sget4(s);

    imgdata.makernotes.sony.Sony0x9400_ReleaseMode2 = SonySubstitution[buf[0x10]];

    imgdata.makernotes.sony.Sony0x9400_SequenceLength1 = SonySubstitution[buf[0x1e]];
  }

  else if ((bufx == 0x0a) && (len >= 0x23))
  { // 0x9400 'a' version
    imgdata.makernotes.sony.Sony0x9400_version = 0xa;

    FORC4 s[c] = SonySubstitution[buf[0x08 + c]];
    imgdata.makernotes.sony.Sony0x9400_SequenceImageNumber = sget4(s);

    FORC4 s[c] = SonySubstitution[buf[0x0c + c]];
    imgdata.makernotes.sony.Sony0x9400_SequenceFileNumber = sget4(s);

    imgdata.makernotes.sony.Sony0x9400_ReleaseMode2 = SonySubstitution[buf[0x10]];

    imgdata.makernotes.sony.Sony0x9400_SequenceLength1 = SonySubstitution[buf[0x22]];
  }

  else
    return;
}