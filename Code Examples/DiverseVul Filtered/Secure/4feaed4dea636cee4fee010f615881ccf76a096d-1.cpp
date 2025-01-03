void LibRaw::identify()
{
  // clang-format off
  static const ushort canon[][11] = {
      // raw_width, raw_height, left_margin, top_margin, width_decrement,
      // height_decrement, mask01, mask03, mask11,
	  // mask13, CFA_filters.
	  { 1944, 1416, 0, 0, 48, 0 }, // 00 "PowerShot Pro90 IS"
	  { 2144, 1560, 4, 8, 52, 2, 0, 0, 0, 25 }, // 01 "PowerShot S30", "PowerShot G1"
	  { 2224, 1456, 48, 6, 0, 2 }, // 02 "EOS D30"
	  { 2376, 1728, 12, 6, 52, 2 }, // 03 "PowerShot G2", "PowerShot S40", "PowerShot G3", "PowerShot S45"
	  { 2672, 1968, 12, 6, 44, 2 }, // 04 "PowerShot G5", "PowerShot S50", "PowerShot S60"
	  { 3152, 2068, 64, 12, 0, 0, 16 }, // 05 "EOS D60", "EOS 10D", "EOS 300D"
	  { 3160, 2344, 44, 12, 4, 4 }, // 06 "PowerShot G6", "PowerShot S70"
	  { 3344, 2484, 4, 6, 52, 6 }, // 07 "PowerShot Pro1"
	  { 3516, 2328, 42, 14, 0, 0 }, // 08 "EOS 350D"
	  { 3596, 2360, 74, 12, 0, 0 }, // 09 "EOS-1D Mark II", "EOS 20D", "EOS-1D Mark II N", "EOS 30D"
	  { 3744, 2784, 52, 12, 8, 12 }, // 10 "PowerShot G11", "PowerShot S90", "PowerShot G12", "PowerShot S95"
	  { 3944, 2622, 30, 18, 6, 2 }, // 11 "EOS 40D"
	  { 3948, 2622, 42, 18, 0, 2 }, // 12 "EOS 400D", "EOS 1000D"
	  { 3984, 2622, 76, 20, 0, 2, 14 }, // 13 "EOS-1D Mark III"
	  { 4032, 2656, 112, 44, 10, 0 }, // 14 APS-C crop mode: "EOS 6D Mark II"??, "EOS RP"
	  { 4104, 3048, 48, 12, 24, 12 }, // 15 "PowerShot G9"
	  { 4116, 2178, 4, 2, 0, 0 },  // 16 ??
	  { 4152, 2772, 192, 12, 0, 0 }, // 17 "PowerShot SX1 IS"
	  { 4160, 3124, 104, 11, 8, 65 }, // 18 "PowerShot S100 (new)", "PowerShot S100V", "PowerShot G15", "PowerShot S110 (new)"
	  { 4176, 3062, 96, 17, 8, 0, 0, 16, 0, 7, 0x49 }, // 19 "PowerShot SX50 HS"
	  { 4192, 3062, 96, 17, 24, 0, 0, 16, 0, 0, 0x49 }, // 20 "PowerShot G16", "PowerShot S120"
	  { 4312, 2876, 22, 18, 0, 2 }, // 21 "EOS 450D"
	  { 4352, 2850, 144, 46, 0, 0 }, // 22 APS-C crop mode: "EOS R"
	  { 4352, 2874, 62, 18, 0, 0 }, // 23 "EOS 1100D"
	  { 4476, 2954, 90, 34, 0, 0 }, // 24 "EOS 5D"
	  { 4480, 3348, 12, 10, 36, 12, 0, 0, 0, 18, 0x49 }, // 25 "PowerShot G10"
	  { 4480, 3366, 80, 50, 0, 0 }, // 26 "PowerShot G1 X Mark II"
	  { 4496, 3366, 80, 50, 12, 0 }, // 27 "PowerShot G1 X"
	  { 4768, 3516, 96, 16, 0, 0, 0, 16 }, // 28 "PowerShot SX60 HS"
	  { 4832, 3204, 62, 26, 0, 0 }, // 29 "EOS 500D"
	  { 4832, 3228, 62, 51, 0, 0 }, // 30 "EOS 50D"
	  { 5108, 3349, 98, 13, 0, 0 }, // 31 "EOS-1Ds Mark II"
	  { 5120, 3318, 142, 45, 62, 0 }, // 32  "EOS-1D Mark IV"
	  { 5280, 3528, 72, 52, 0, 0 }, // 33 "EOS M10", "EOS 650D", "EOS 700D", "EOS M", "EOS 100D", "EOS M2"
	  { 5344, 3516, 142, 51, 0, 0 }, // 34 "EOS 550D", "EOS 600D", "EOS 60D", "EOS 1200D", "EOS 1300D", "EOS 3000D"
	  { 5344, 3584, 126, 100, 0, 2 }, // 35 "EOS-1D X", "EOS-1D C"
	  { 5344, 3950, 98, 18, 0, 0, 0, 24, 0, 0 }, // 36 "PowerShot SX70 HS"
	  { 5360, 3516, 158, 51, 0, 0 }, // 37 "EOS 7D"
	  { 5568, 3708, 72, 38, 0, 0 }, // 38; "EOS 7D Mark II", "EOS 6D", "EOS 70D", "EOS-1D X MARK II"
	  { 5632, 3710, 96, 17, 0, 0, 0, 16, 0, 0, 0x49 }, // 39 "PowerShot G7 X", "PowerShot G3 X", "PowerShot G9 X", "PowerShot G5 X", "PowerShot G7 X Mark II", "PowerShot G9 X Mark II"
	  { 5712, 3774, 62, 20, 10, 2 }, // 40 "EOS-1Ds Mark III"
	  { 5792, 3804, 158, 51, 0, 0 }, // 41 "EOS 5D Mark II"
	  { 5920, 3950, 122, 80, 2, 0 }, // 42 "EOS 5D Mark III"
	  { 6096, 4051, 76, 35, 0, 0 }, // 43 "EOS 1500D"
	  { 6096, 4056, 72, 34, 0, 0 }, // 44 "EOS M3", "EOS 760D", "EOS 750D"
	  { 6288, 4056, 264, 36, 0, 0 }, // 45 "EOS M5", "EOS M100", "EOS M6", "PowerShot G1 X Mark III", "EOS 80D", "EOS 800D", "EOS 77D", "EOS 200D", "EOS 250D", "EOS M50"
	  { 6384, 4224, 120, 44, 0, 0 }, // 46 "EOS 6D Mark II", "EOS RP"
	  { 6880, 4544, 136, 42, 0, 0 }, // 47 "EOS 5D Mark IV"
	  { 6888, 4546, 146, 48, 0, 0 }, // 48 "EOS R"
	  { 7128, 4732, 144, 72, 0, 0 }, // 49 "EOS M6 II", "EOS 90D"
	  { 8896, 5920, 160, 64, 0, 0 }, // 50 "EOS 5DS", "EOS 5DS R"
  };

  static const libraw_custom_camera_t const_table[] = {
	  { 786432, 1024, 768, 0, 0, 0, 0, 0, 0x94, 0, 0, "AVT", "F-080C" },
	  { 1447680, 1392, 1040, 0, 0, 0, 0, 0, 0x94, 0, 0, "AVT", "F-145C" },
	  { 1920000, 1600, 1200, 0, 0, 0, 0, 0, 0x94, 0, 0, "AVT", "F-201C" },
	  { 5067304, 2588, 1958, 0, 0, 0, 0, 0, 0x94, 0, 0, "AVT", "F-510C" },
	  { 5067316, 2588, 1958, 0, 0, 0, 0, 0, 0x94, 0, 0, "AVT", "F-510C", 12 },
	  { 10134608, 2588, 1958, 0, 0, 0, 0, 9, 0x94, 0, 0, "AVT", "F-510C" },
	  { 10134620, 2588, 1958, 0, 0, 0, 0, 9, 0x94, 0, 0, "AVT", "F-510C", 12 },
	  { 16157136, 3272, 2469, 0, 0, 0, 0, 9, 0x94, 0, 0, "AVT", "F-810C" },
	  { 15980544, 3264, 2448, 0, 0, 0, 0, 8, 0x61, 0, 1, "AgfaPhoto", "DC-833m" },
	  { 9631728, 2532, 1902, 0, 0, 0, 0, 96, 0x61, 0, 0, "Alcatel", "5035D" },
	  { 31850496, 4608, 3456, 0, 0, 0, 0, 0, 0x94, 0, 0, "GITUP", "GIT2 4:3" },
	  { 23887872, 4608, 2592, 0, 0, 0, 0, 0, 0x94, 0, 0, "GITUP", "GIT2 16:9" },
	  { 32257024, 4624, 3488, 8, 2, 16, 2, 0, 0x94, 0, 0, "GITUP", "GIT2P 4:3" },
	  { 24192768, 4624, 2616, 8, 2, 16, 2, 0, 0x94, 0, 0, "GITUP", "GIT2P 16:9" },
	  { 18016000, 4000, 2252, 0, 0, 0, 0, 0, 0x94, 0, 0, "GITUP", "G3DUO 16:9" },
	  //          {24000000, 4000, 3000, 0, 0, 0, 0, 0, 0x94, 0, 0, "GITUP",
      //          "G3DUO 4:3"}, // Conflict w/ Samsung WB550

      //   Android Raw dumps id start
      //   File Size in bytes Horizontal Res Vertical Flag then bayer order eg
      //   0x16 bbgr 0x94 rggb
	  { 1540857, 2688, 1520, 0, 0, 0, 0, 1, 0x61, 0, 0, "Samsung", "S3" },
	  { 2658304, 1212, 1096, 0, 0, 0, 0, 1, 0x16, 0, 0, "LG", "G3FrontMipi" },
	  { 2842624, 1296, 1096, 0, 0, 0, 0, 1, 0x16, 0, 0, "LG", "G3FrontQCOM" },
	  { 2969600, 1976, 1200, 0, 0, 0, 0, 1, 0x16, 0, 0, "Xiaomi", "MI3wMipi" },
	  { 3170304, 1976, 1200, 0, 0, 0, 0, 1, 0x16, 0, 0, "Xiaomi", "MI3wQCOM" },
	  { 3763584, 1584, 1184, 0, 0, 0, 0, 96, 0x61, 0, 0, "I_Mobile", "I_StyleQ6" },
	  { 5107712, 2688, 1520, 0, 0, 0, 0, 1, 0x61, 0, 0, "OmniVisi", "UltraPixel1" },
	  { 5382640, 2688, 1520, 0, 0, 0, 0, 1, 0x61, 0, 0, "OmniVisi", "UltraPixel2" },
	  { 5664912, 2688, 1520, 0, 0, 0, 0, 1, 0x61, 0, 0, "OmniVisi", "4688" },
	  { 5664912, 2688, 1520, 0, 0, 0, 0, 1, 0x61, 0, 0, "OmniVisi", "4688" },
	  { 5364240, 2688, 1520, 0, 0, 0, 0, 1, 0x61, 0, 0, "OmniVisi", "4688" },
	  { 6299648, 2592, 1944, 0, 0, 0, 0, 1, 0x16, 0, 0, "OmniVisi", "OV5648" },
	  { 6721536, 2592, 1944, 0, 0, 0, 0, 0, 0x16, 0, 0, "OmniVisi", "OV56482" },
	  { 6746112, 2592, 1944, 0, 0, 0, 0, 0, 0x16, 0, 0, "HTC", "OneSV" },
	  { 9631728, 2532, 1902, 0, 0, 0, 0, 96, 0x61, 0, 0, "Sony", "5mp" },
	  { 9830400, 2560, 1920, 0, 0, 0, 0, 96, 0x61, 0, 0, "NGM", "ForwardArt" },
	  { 10186752, 3264, 2448, 0, 0, 0, 0, 1, 0x94, 0, 0, "Sony", "IMX219-mipi 8mp" },
	  { 10223360, 2608, 1944, 0, 0, 0, 0, 96, 0x16, 0, 0, "Sony", "IMX" },
	  { 10782464, 3282, 2448, 0, 0, 0, 0, 0, 0x16, 0, 0, "HTC", "MyTouch4GSlide" },
	  { 10788864, 3282, 2448, 0, 0, 0, 0, 0, 0x16, 0, 0, "Xperia", "L" },
	  { 15967488, 3264, 2446, 0, 0, 0, 0, 96, 0x16, 0, 0, "OmniVison", "OV8850" },
	  { 16224256, 4208, 3082, 0, 0, 0, 0, 1, 0x16, 0, 0, "LG", "G3MipiL" },
	  { 16424960, 4208, 3120, 0, 0, 0, 0, 1, 0x16, 0, 0, "IMX135", "MipiL" },
	  { 17326080, 4164, 3120, 0, 0, 0, 0, 1, 0x16, 0, 0, "LG", "G3LQCom" },
	  { 17522688, 4212, 3120, 0, 0, 0, 0, 0, 0x16, 0, 0, "Sony", "IMX135-QCOM" },
	  { 19906560, 4608, 3456, 0, 0, 0, 0, 1, 0x16, 0, 0, "Gione", "E7mipi" },
	  { 19976192, 5312, 2988, 0, 0, 0, 0, 1, 0x16, 0, 0, "LG", "G4" },
	  { 20389888, 4632, 3480, 0, 0, 0, 0, 1, 0x16, 0, 0, "Xiaomi", "RedmiNote3Pro" },
	  { 20500480, 4656, 3496, 0, 0, 0, 0, 1, 0x94, 0, 0, "Sony", "IMX298-mipi 16mp" },
	  { 21233664, 4608, 3456, 0, 0, 0, 0, 1, 0x16, 0, 0, "Gione", "E7qcom" },
	  { 26023936, 4192, 3104, 0, 0, 0, 0, 96, 0x94, 0, 0, "THL", "5000" },
	  { 26257920, 4208, 3120, 0, 0, 0, 0, 96, 0x94, 0, 0, "Sony", "IMX214" },
	  { 26357760, 4224, 3120, 0, 0, 0, 0, 96, 0x61, 0, 0, "OV", "13860" },
	  { 41312256, 5248, 3936, 0, 0, 0, 0, 96, 0x61, 0, 0, "Meizu", "MX4" },
	  { 42923008, 5344, 4016, 0, 0, 0, 0, 96, 0x61, 0, 0, "Sony", "IMX230" },
      //   Android Raw dumps id end
	  { 20137344, 3664, 2748, 0, 0, 0, 0, 0x40, 0x49, 0, 0, "Aptina", "MT9J003", 0xffff },
	  { 2868726, 1384, 1036, 0, 0, 0, 0, 64, 0x49, 0, 8, "Baumer", "TXG14", 1078 },
	  { 5298000, 2400, 1766, 12, 12, 44, 2, 40, 0x94, 0, 2, "Canon", "PowerShot SD300" }, // chdk hack
	  { 6553440, 2664, 1968, 4, 4, 44, 4, 40, 0x94, 0, 2, "Canon", "PowerShot A460" }, // chdk hack
	  { 6573120, 2672, 1968, 12, 8, 44, 0, 40, 0x94, 0, 2, "Canon", "PowerShot A610" }, // chdk hack
	  { 6653280, 2672, 1992, 10, 6, 42, 2, 40, 0x94, 0, 2, "Canon", "PowerShot A530" }, // chdk hack
	  { 7710960, 2888, 2136, 44, 8, 4, 0, 40, 0x94, 0, 2, "Canon", "PowerShot S3 IS" }, // chdk hack
	  { 9219600, 3152, 2340, 36, 12, 4, 0, 40, 0x94, 0, 2, "Canon", "PowerShot A620" }, // chdk hack
	  { 9243240, 3152, 2346, 12, 7, 44, 13, 40, 0x49, 0, 2, "Canon", "PowerShot A470" }, // chdk hack
	  { 10341600, 3336, 2480, 6, 5, 32, 3, 40, 0x94, 0, 2, "Canon", "PowerShot A720 IS" }, // chdk hack
	  { 10383120, 3344, 2484, 12, 6, 44, 6, 40, 0x94, 0, 2, "Canon", "PowerShot A630" }, // chdk hack
	  { 12945240, 3736, 2772, 12, 6, 52, 6, 40, 0x94, 0, 2, "Canon", "PowerShot A640" }, // chdk hack
	  { 15636240, 4104, 3048, 48, 12, 24, 12, 40, 0x94, 0, 2, "Canon", "PowerShot A650" }, // chdk hack
	  { 15467760, 3720, 2772, 6, 12, 30, 0, 40, 0x94, 0, 2, "Canon", "PowerShot SX110 IS" }, // chdk hack
	  { 15534576, 3728, 2778, 12, 9, 44, 9, 40, 0x94, 0, 2, "Canon", "PowerShot SX120 IS" }, // chdk hack
	  { 18653760, 4080, 3048, 24, 12, 24, 12, 40, 0x94, 0, 2, "Canon", "PowerShot SX20 IS" }, // chdk hack
	  { 18763488, 4104, 3048, 10, 22, 82, 22, 8, 0x49, 0, 0, "Canon", "PowerShot D10" }, // ? chdk hack ?
	  { 19131120, 4168, 3060, 92, 16, 4, 1, 40, 0x94, 0, 2, "Canon", "PowerShot SX220 HS" }, // chdk hack
	  { 21936096, 4464, 3276, 25, 10, 73, 12, 40, 0x16, 0, 2, "Canon", "PowerShot SX30 IS" }, // chdk hack
	  { 24724224, 4704, 3504, 8, 16, 56, 8, 40, 0x49, 0, 2, "Canon", "PowerShot A3300 IS" }, // chdk hack
	  { 30858240, 5248, 3920, 8, 16, 56, 16, 40, 0x94, 0, 2, "Canon", "IXUS 160" }, // chdk hack
	  { 1976352, 1632, 1211, 0, 2, 0, 1, 0, 0x94, 0, 1, "Casio", "QV-2000UX" },
	  { 3217760, 2080, 1547, 0, 0, 10, 1, 0, 0x94, 0, 1, "Casio", "QV-3*00EX" },
	  { 6218368, 2585, 1924, 0, 0, 9, 0, 0, 0x94, 0, 1, "Casio", "QV-5700" },
	  { 7816704, 2867, 2181, 0, 0, 34, 36, 0, 0x16, 0, 1, "Casio", "EX-Z60" },
	  { 2937856, 1621, 1208, 0, 0, 1, 0, 0, 0x94, 7, 13, "Casio", "EX-S20" },
	  { 4948608, 2090, 1578, 0, 0, 32, 34, 0, 0x94, 7, 1, "Casio", "EX-S100" },
	  { 6054400, 2346, 1720, 2, 0, 32, 0, 0, 0x94, 7, 1, "Casio", "QV-R41" },
	  { 7426656, 2568, 1928, 0, 0, 0, 0, 0, 0x94, 0, 1, "Casio", "EX-P505" },
	  { 7530816, 2602, 1929, 0, 0, 22, 0, 0, 0x94, 7, 1, "Casio", "QV-R51" },
	  { 7542528, 2602, 1932, 0, 0, 32, 0, 0, 0x94, 7, 1, "Casio", "EX-Z50" },
	  { 7562048, 2602, 1937, 0, 0, 25, 0, 0, 0x16, 7, 1, "Casio", "EX-Z500" },
	  { 7753344, 2602, 1986, 0, 0, 32, 26, 0, 0x94, 7, 1, "Casio", "EX-Z55" },
	  { 9313536, 2858, 2172, 0, 0, 14, 30, 0, 0x94, 7, 1, "Casio", "EX-P600" },
	  { 10834368, 3114, 2319, 0, 0, 27, 0, 0, 0x94, 0, 1, "Casio", "EX-Z750" },
	  { 10843712, 3114, 2321, 0, 0, 25, 0, 0, 0x94, 0, 1, "Casio", "EX-Z75" },
	  { 10979200, 3114, 2350, 0, 0, 32, 32, 0, 0x94, 7, 1, "Casio", "EX-P700" },
	  { 12310144, 3285, 2498, 0, 0, 6, 30, 0, 0x94, 0, 1, "Casio", "EX-Z850" },
	  { 12489984, 3328, 2502, 0, 0, 47, 35, 0, 0x94, 0, 1, "Casio", "EX-Z8" },
	  { 15499264, 3754, 2752, 0, 0, 82, 0, 0, 0x94, 0, 1, "Casio", "EX-Z1050" },
	  { 18702336, 4096, 3044, 0, 0, 24, 0, 80, 0x94, 7, 1, "Casio", "EX-ZR100" },
	  { 7684000, 2260, 1700, 0, 0, 0, 0, 13, 0x94, 0, 1, "Casio", "QV-4000" },
	  { 787456, 1024, 769, 0, 1, 0, 0, 0, 0x49, 0, 0, "Creative", "PC-CAM 600" },
	  { 28829184, 4384, 3288, 0, 0, 0, 0, 36, 0x61, 0, 0, "DJI" },
	  { 15151104, 4608, 3288, 0, 0, 0, 0, 0, 0x94, 0, 0, "Matrix" },
	  { 3840000, 1600, 1200, 0, 0, 0, 0, 65, 0x49, 0, 0, "Foculus", "531C" },
	  { 307200, 640, 480, 0, 0, 0, 0, 0, 0x94, 0, 0, "Generic" },
	  { 62464, 256, 244, 1, 1, 6, 1, 0, 0x8d, 0, 0, "Kodak", "DC20" },
	  { 124928, 512, 244, 1, 1, 10, 1, 0, 0x8d, 0, 0, "Kodak", "DC20" },
	  { 1652736, 1536, 1076, 0, 52, 0, 0, 0, 0x61, 0, 0, "Kodak", "DCS200" },
	  { 4159302, 2338, 1779, 1, 33, 1, 2, 0, 0x94, 0, 0, "Kodak", "C330" },
	  { 4162462, 2338, 1779, 1, 33, 1, 2, 0, 0x94, 0, 0, "Kodak", "C330", 3160 },
	  { 2247168, 1232, 912, 0, 0, 16, 0, 0, 0x00, 0, 0, "Kodak", "C330" },
	  { 3370752, 1232, 912, 0, 0, 16, 0, 0, 0x00, 0, 0, "Kodak", "C330" },
	  { 6163328, 2864, 2152, 0, 0, 0, 0, 0, 0x94, 0, 0, "Kodak", "C603" },
	  { 6166488, 2864, 2152, 0, 0, 0, 0, 0, 0x94, 0, 0, "Kodak", "C603", 3160 },
	  { 460800, 640, 480, 0, 0, 0, 0, 0, 0x00, 0, 0, "Kodak", "C603" },
	  { 9116448, 2848, 2134, 0, 0, 0, 0, 0, 0x00, 0, 0, "Kodak", "C603" },
	  { 12241200, 4040, 3030, 2, 0, 0, 13, 0, 0x49, 0, 0, "Kodak", "12MP" },
	  { 12272756, 4040, 3030, 2, 0, 0, 13, 0, 0x49, 0, 0, "Kodak", "12MP", 31556 },
	  { 18000000, 4000, 3000, 0, 0, 0, 0, 0, 0x00, 0, 0, "Kodak", "12MP" },
	  { 614400, 640, 480, 0, 3, 0, 0, 64, 0x94, 0, 0, "Kodak", "KAI-0340" },
	  { 15360000, 3200, 2400, 0, 0, 0, 0, 96, 0x16, 0, 0, "Lenovo", "A820" },
	  { 3884928, 1608, 1207, 0, 0, 0, 0, 96, 0x16, 0, 0, "Micron", "2010", 3212 },
	  { 1138688, 1534, 986, 0, 0, 0, 0, 0, 0x61, 0, 0, "Minolta", "RD175", 513 },
	  { 1581060, 1305, 969, 0, 0, 18, 6, 6, 0x1e, 4, 1, "Nikon", "E900" }, // "diag raw" hack
	  { 2465792, 1638, 1204, 0, 0, 22, 1, 6, 0x4b, 5, 1, "Nikon", "E950" }, // "diag raw" hack; possibly also Nikon E700, E800, E775;
	                                                                        // Olympus C-2020Z
	  { 2940928, 1616, 1213, 0, 0, 0, 7, 30, 0x94, 0, 1, "Nikon", "E2100" }, // "diag raw" hack; also Nikon E2500
	  { 4771840, 2064, 1541, 0, 0, 0, 1, 6, 0xe1, 0, 1, "Nikon", "E990" }, // "diag raw" hack; possibly also Nikon E880, E885, E995;
	                                                                       // Olympus C-3030Z
	  { 4775936, 2064, 1542, 0, 0, 0, 0, 30, 0x94, 0, 1, "Nikon", "E3700" }, // "diag raw" hack; Nikon E3100, E3200, E3500;
	                                                                         // Pentax "Optio 33WR"; possibly also Olympus C-740UZ
	  { 5865472, 2288, 1709, 0, 0, 0, 1, 6, 0xb4, 0, 1, "Nikon", "E4500" }, // "diag raw" hack; possibly also Olympus C-4040Z
	  { 5869568, 2288, 1710, 0, 0, 0, 0, 6, 0x16, 0, 1, "Nikon", "E4300" }, // "diag raw" hack; also Minolta "DiMAGE Z2"
	  { 7438336, 2576, 1925, 0, 0, 0, 1, 6, 0xb4, 0, 1, "Nikon", "E5000" }, // also Nikon E5700
	  { 8998912, 2832, 2118, 0, 0, 0, 0, 30, 0x94, 7, 1, "Nikon", "COOLPIX S6" }, // "diag raw" hack
	  { 5939200, 2304, 1718, 0, 0, 0, 0, 30, 0x16, 0, 0, "Olympus", "C-770UZ" }, // possibly also Olympus C-4100Z, C-765UZ
	  { 3178560, 2064, 1540, 0, 0, 0, 0, 0, 0x94, 0, 1, "Pentax", "Optio S V1.01" },
	  { 4841984, 2090, 1544, 0, 0, 22, 0, 0, 0x94, 7, 1, "Pentax", "Optio S" },
	  { 6114240, 2346, 1737, 0, 0, 22, 0, 0, 0x94, 7, 1, "Pentax", "Optio S4" },
	  { 10702848, 3072, 2322, 0, 0, 0, 21, 30, 0x94, 0, 1, "Pentax", "Optio 750Z" },
	  { 4147200, 1920, 1080, 0, 0, 0, 0, 0, 0x49, 0, 0, "Photron", "BC2-HD" },
	  { 4151666, 1920, 1080, 0, 0, 0, 0, 0, 0x49, 0, 0, "Photron", "BC2-HD", 8 },
	  { 13248000, 2208, 3000, 0, 0, 0, 0, 13, 0x61, 0, 0, "Pixelink", "A782" },
	  { 6291456, 2048, 1536, 0, 0, 0, 0, 96, 0x61, 0, 0, "RoverShot", "3320AF" },
	  { 311696, 644, 484, 0, 0, 0, 0, 0, 0x16, 0, 8, "ST Micro", "STV680 VGA" },
	  { 16098048, 3288, 2448, 0, 0, 24, 0, 9, 0x94, 0, 1, "Samsung", "S85" }, // hack
	  { 16215552, 3312, 2448, 0, 0, 48, 0, 9, 0x94, 0, 1, "Samsung", "S85" }, // hack
	  { 20487168, 3648, 2808, 0, 0, 0, 0, 13, 0x94, 5, 1, "Samsung", "WB550" },
	  { 24000000, 4000, 3000, 0, 0, 0, 0, 13, 0x94, 5, 1, "Samsung", "WB550" },
	  { 12582980, 3072, 2048, 0, 0, 0, 0, 33, 0x61, 0, 0, "Sinar", "", 68 }, // Sinarback 23; same res. as Leaf Volare & Cantare
	  { 33292868, 4080, 4080, 0, 0, 0, 0, 33, 0x61, 0, 0, "Sinar", "", 68 }, // Sinarback 44
	  { 44390468, 4080, 5440, 0, 0, 0, 0, 33, 0x61, 0, 0, "Sinar", "", 68 }, // Sinarback 54
	  { 1409024, 1376, 1024, 0, 0, 1, 0, 0, 0x49, 0, 0, "Sony", "XCD-SX910CR" },
	  { 2818048, 1376, 1024, 0, 0, 1, 0, 97, 0x49, 0, 0, "Sony", "XCD-SX910CR" },
  };

  libraw_custom_camera_t
      table[64 + sizeof(const_table) / sizeof(const_table[0])];


  // clang-format on

  char head[64] = {0}, *cp;
  int hlen, fsize, flen, zero_fsize = 1, i, c;
  struct jhead jh;

  unsigned camera_count =
      parse_custom_cameras(64, table, imgdata.params.custom_camera_strings);
  for (int q = 0; q < int(sizeof(const_table) / sizeof(const_table[0])); q++)
    memmove(&table[q + camera_count], &const_table[q], sizeof(const_table[0]));
  camera_count += sizeof(const_table) / sizeof(const_table[0]);

  tiff_flip = flip = filters = UINT_MAX; /* unknown */
  raw_height = raw_width = fuji_width = fuji_layout = cr2_slice[0] = 0;
  maximum = height = width = top_margin = left_margin = 0;
  cdesc[0] = desc[0] = artist[0] = make[0] = model[0] = model2[0] = 0;
  iso_speed = shutter = aperture = focal_len = 0;
  unique_id = 0ULL;
  tiff_nifds = 0;
  is_NikonTransfer = 0;
  is_Sony = 0;
  is_pana_raw = 0;
  maker_index = LIBRAW_CAMERAMAKER_Unknown;
  is_4K_RAFdata = 0;
  FujiCropMode = 0;
  is_PentaxRicohMakernotes = 0;
  normalized_model[0] = 0;
  normalized_make[0] = 0;
  CM_found = 0;
  memset(tiff_ifd, 0, sizeof tiff_ifd);
  libraw_internal_data.unpacker_data.crx_track_selected = -1;
  libraw_internal_data.unpacker_data.CR3_CTMDtag = 0;
  imgdata.makernotes.hasselblad.nIFD_CM[0] =
    imgdata.makernotes.hasselblad.nIFD_CM[1] = -1;
  imgdata.makernotes.kodak.ISOCalibrationGain = 1.0f;
  imCommon.CameraTemperature = imCommon.SensorTemperature =
      imCommon.SensorTemperature2 = imCommon.LensTemperature =
          imCommon.AmbientTemperature = imCommon.BatteryTemperature =
              imCommon.exifAmbientTemperature = -1000.0f;

  imgdata.color.ExifColorSpace = LIBRAW_COLORSPACE_Unknown;
  for (i = 0; i < LIBRAW_IFD_MAXCOUNT; i++)
  {
    tiff_ifd[i].dng_color[0].illuminant = tiff_ifd[i].dng_color[1].illuminant =
        0xffff;
    for (int c = 0; c < 4; c++)
      tiff_ifd[i].dng_levels.analogbalance[c] = 1.0f;
  }

  memset(gpsdata, 0, sizeof gpsdata);
  memset(cblack, 0, sizeof cblack);
  memset(white, 0, sizeof white);
  memset(mask, 0, sizeof mask);
  thumb_offset = thumb_length = thumb_width = thumb_height = 0;
  load_raw = thumb_load_raw = 0;
  write_thumb = &LibRaw::jpeg_thumb;
  data_offset = meta_offset = meta_length = tiff_bps = tiff_compress = 0;
  kodak_cbpp = zero_after_ff = dng_version = load_flags = 0;
  timestamp = shot_order = tiff_samples = black = is_foveon = 0;
  mix_green = profile_length = data_error = zero_is_bad = 0;
  pixel_aspect = is_raw = raw_color = 1;
  tile_width = tile_length = 0;
  metadata_blocks = 0;

  for (i = 0; i < 4; i++)
  {
    cam_mul[i] = i == 1;
    pre_mul[i] = i < 3;
    FORC3 cmatrix[c][i] = 0;
    FORC3 rgb_cam[c][i] = c == i;
  }
  colors = 3;
  for (i = 0; i < 0x10000; i++)
    curve[i] = i;

  order = get2();
  hlen = get4();
  fseek(ifp, 0, SEEK_SET);

  if (fread(head, 1, 64, ifp) < 64)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  libraw_internal_data.unpacker_data.lenRAFData =
      libraw_internal_data.unpacker_data.posRAFData = 0;

  fseek(ifp, 0, SEEK_END);
  flen = fsize = ftell(ifp);
  if ((cp = (char *)memmem(head, 32, (char *)"MMMM", 4)) ||
      (cp = (char *)memmem(head, 32, (char *)"IIII", 4)))
  {
    parse_phase_one(cp - head);
    if (cp - head && parse_tiff(0))
      apply_tiff();
  }
  else if (order == 0x4949 || order == 0x4d4d)
  {
    if (!memcmp(head + 6, "HEAPCCDR", 8))
    {
      data_offset = hlen;
      parse_ciff(hlen, flen - hlen, 0);
      load_raw = &LibRaw::canon_load_raw;
    }
    else if (parse_tiff(0))
      apply_tiff();
  }
  else if (!memcmp(head, "\xff\xd8\xff\xe1", 4) && !memcmp(head + 6, "Exif", 4))
  {
    fseek(ifp, 4, SEEK_SET);
    data_offset = 4 + get2();
    fseek(ifp, data_offset, SEEK_SET);
    if (fgetc(ifp) != 0xff)
      parse_tiff(12);
    thumb_offset = 0;
  }
  else if (!memcmp(head + 25, "ARECOYK", 7)) // 'KYOCERA' right-to-left
  {
    strcpy(make, "Contax");
    strcpy(model, "N Digital");
    parse_kyocera();
  }
  else if (!strcmp(head, "PXN"))
  {
    strcpy(make, "Logitech");
    strcpy(model, "Fotoman Pixtura");
  }
  else if (!strcmp(head, "qktk"))
  {
    strcpy(make, "Apple");
    strcpy(model, "QuickTake 100");
    load_raw = &LibRaw::quicktake_100_load_raw;
  }
  else if (!strcmp(head, "qktn"))
  {
    strcpy(make, "Apple");
    strcpy(model, "QuickTake 150");
    load_raw = &LibRaw::kodak_radc_load_raw;
  }
  else if (!memcmp(head, "FUJIFILM", 8))
  {
    memcpy(imFuji.SerialSignature, head + 0x10, 0x0c);
    imFuji.SerialSignature[0x0c] = 0;
    strncpy(model, head + 0x1c, 0x20);
    model[0x20] = 0;
    memcpy(model2, head + 0x3c, 4);
    model2[4] = 0;
    strcpy(imFuji.RAFVersion, model2);
    fseek(ifp, 84, SEEK_SET);
    thumb_offset = get4();
    thumb_length = get4();
    fseek(ifp, 92, SEEK_SET);
    parse_fuji(get4());
    if (thumb_offset > 120)
    {
      fseek(ifp, 120, SEEK_SET);
      is_raw += (i = get4()) ? 1 : 0;
      if (is_raw == 2 && shot_select)
        parse_fuji(i);
    }
    load_raw = &LibRaw::unpacked_load_raw;
    fseek(ifp, 100 + 28 * (shot_select > 0), SEEK_SET);
    parse_tiff(data_offset = get4());
    parse_tiff(thumb_offset + 12);
    apply_tiff();
  }
  else if (!memcmp(head, "RIFF", 4))
  {
    fseek(ifp, 0, SEEK_SET);
    parse_riff();
  }
  else if (!memcmp(head + 4, "ftypqt   ", 9))
  {
    fseek(ifp, 0, SEEK_SET);
    parse_qt(fsize);
    is_raw = 0;
  }
  else if (!memcmp(head, "\0\001\0\001\0@", 6))
  {
    fseek(ifp, 6, SEEK_SET);
    fread(make, 1, 8, ifp);
    fread(model, 1, 8, ifp);
    fread(model2, 1, 16, ifp);
    data_offset = get2();
    get2();
    raw_width = get2();
    raw_height = get2();
    load_raw = &LibRaw::nokia_load_raw;
    filters = 0x61616161;
  }
  else if (!memcmp(head, "NOKIARAW", 8))
  {
    strcpy(make, "NOKIA");
    order = 0x4949;
    fseek(ifp, 300, SEEK_SET);
    data_offset = get4();
    i = get4(); // bytes count
    width = get2();
    height = get2();

    // Data integrity check
    if (width < 1 || width > 16000 || height < 1 || height > 16000 ||
        i < (width * height) || i > (2 * width * height))
      throw LIBRAW_EXCEPTION_IO_CORRUPT;

    switch (tiff_bps = i * 8 / (width * height))
    {
    case 8:
      load_raw = &LibRaw::eight_bit_load_raw;
      break;
    case 10:
      load_raw = &LibRaw::nokia_load_raw;
      break;
    case 0:
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
      break;
    }
    raw_height = height + (top_margin = i / (width * tiff_bps / 8) - height);
    mask[0][3] = 1;
    filters = 0x61616161;
  }
  else if (!memcmp(head, "ARRI", 4))
  {
    order = 0x4949;
    fseek(ifp, 20, SEEK_SET);
    width = get4();
    height = get4();
    strcpy(make, "ARRI");
    fseek(ifp, 668, SEEK_SET);
    fread(model, 1, 64, ifp);
    model[63] = 0;
    fseek(ifp, 760, SEEK_SET);
    fread(software, 1, 64, ifp);
    if((unsigned char)software[0] == 0xff) software[0] = 0;
    software[63] = 0;
    data_offset = 4096;
    load_raw = &LibRaw::packed_load_raw;
    load_flags = 88;
    filters = 0x61616161;
    fixupArri();
  }
  else if (!memcmp(head, "XPDS", 4))
  {
    order = 0x4949;
    fseek(ifp, 0x800, SEEK_SET);
    fread(make, 1, 41, ifp);
    raw_height = get2();
    raw_width = get2();
    fseek(ifp, 56, SEEK_CUR);
    fread(model, 1, 30, ifp);
    data_offset = 0x10000;
    load_raw = &LibRaw::canon_rmf_load_raw;
    gamma_curve(0, 12.25, 1, 1023);
  }
  else if (!memcmp(head + 4, "RED1", 4))
  {
    strcpy(make, "Red");
    strcpy(model, "One");
    parse_redcine();
    load_raw = &LibRaw::redcine_load_raw;
    gamma_curve(1 / 2.4, 12.92, 1, 4095);
    filters = 0x49494949;
  }
  else if (!memcmp(head, "DSC-Image", 9))
    parse_rollei();
  else if (!memcmp(head, "PWAD", 4))
    parse_sinar_ia();
  else if (!memcmp(head, "\0MRM", 4))
    parse_minolta(0);
  else if (!memcmp(head, "FOVb", 4))
  {
    parse_x3f(); /* Does nothing if USE_X3FTOOLS is not defined */
  }
  else if (!memcmp(head, "CI", 2))
    parse_cine();
#ifdef USE_6BY9RPI
  else if (!memcmp(head, "BRCM", 4)) {
	fseek(ifp, 0, SEEK_SET);
	strcpy(make, "RaspberryPi");
	strcpy(model, "Pi");
	parse_raspberrypi();
	}
#endif
  else if (!memcmp(head + 4, "ftypcrx ", 8))
  {
    int err;
    unsigned long long szAtomList;
    short nesting = -1;
    short nTrack = -1;
    short TrackType;
    char AtomNameStack[128];
    strcpy(make, "Canon");

    szAtomList = ifp->size();
    err = parseCR3(0ULL, szAtomList, nesting, AtomNameStack, nTrack, TrackType);
    if ((err == 0 || err == -14) &&
        nTrack >= 0) // no error, or too deep nesting
      selectCRXTrack(nTrack);
  }

  if (make[0] == 0)
    for (zero_fsize = i = 0; i < (int)camera_count; i++)
      if (fsize == (int)table[i].fsize)
      {
        strcpy(make, table[i].t_make);
        strcpy(model, table[i].t_model);
        flip = table[i].flags >> 2;
        zero_is_bad = table[i].flags & 2;
        data_offset = table[i].offset == 0xffff ? 0 : table[i].offset;
        raw_width = table[i].rw;
        raw_height = table[i].rh;
        left_margin = table[i].lm;
        top_margin = table[i].tm;
        width = raw_width - left_margin - table[i].rm;
        height = raw_height - top_margin - table[i].bm;
        filters = 0x1010101U * table[i].cf;
        colors = 4 - !((filters & filters >> 1) & 0x5555);
        load_flags = table[i].lf & 0xff;
        if (table[i].lf & 0x100) /* Monochrome sensor dump */
        {
          colors = 1;
          filters = 0;
        }
        switch (tiff_bps = (fsize - data_offset) * 8 / (raw_width * raw_height))
        {
        case 6:
          load_raw = &LibRaw::minolta_rd175_load_raw;
          ilm.CameraMount = LIBRAW_MOUNT_Minolta_A;
          break;
        case 8:
          load_raw = &LibRaw::eight_bit_load_raw;
          break;
        case 10:
          if ((fsize - data_offset) / raw_height * 3 >= raw_width * 4)
          {
            load_raw = &LibRaw::android_loose_load_raw;
            break;
          }
          else if (load_flags & 1)
          {
            load_raw = &LibRaw::android_tight_load_raw;
            break;
          }
        case 12:
          load_flags |= 128;
          load_raw = &LibRaw::packed_load_raw;
          break;
        case 16:
          order = 0x4949 | 0x404 * (load_flags & 1);
          tiff_bps -= load_flags >> 4;
          tiff_bps -= load_flags = load_flags >> 1 & 7;
          load_raw = table[i].offset == 0xffff
                         ? &LibRaw::unpacked_load_raw_reversed
                         : &LibRaw::unpacked_load_raw;
        }
        maximum = (1 << tiff_bps) - (1 << table[i].max);
        break;
      }
  if (zero_fsize)
    fsize = 0;
  if (make[0] == 0)
    parse_smal(0, flen);
  if (make[0] == 0)
  {
    parse_jpeg(0);
#ifdef USE_6BY9RPI
	if (!(strncmp(model, "ov", 2) && strncmp(model, "RP_", 3))) {
		//Assume that this isn't a raw unless the header can be found
		is_raw = 0;

		if (!strncasecmp(model, "RP_imx", 6)) {
			const long offsets[] = {
				//IMX219 offsets
				10270208, //8MPix 3280x2464
				2678784,  //1920x1080
				2628608,  //1640x1232
				1963008,  //1640x922
				1233920,  //1280x720
				445440,   //640x480
				-1        //Marker for end of table
			};
			int offset_idx;
			for (offset_idx = 0; offsets[offset_idx] != -1; offset_idx++) {
				if (!fseek(ifp, -offsets[offset_idx], SEEK_END) &&
					fread(head, 1, 32, ifp) && !strncmp(head, "BRCM", 4)) {

					fseek(ifp, -32, SEEK_CUR);
					strcpy(make, "SonyRPF");
					parse_raspberrypi();
					break;
				}
			}
		}
		else if (!strncasecmp(model, "RP_OV", 5) || !strncasecmp(model, "ov5647", 6)) {
			const long offsets[] = {
					6404096,  //5MPix 2592x1944
					2717696,  //1920x1080
					1625600,  //1296x972
					1233920,  //1296x730
					445440,   //640x480
					-1        //Marker for end of table
			};
			int offset_idx;
			for (offset_idx = 0; offsets[offset_idx] != -1; offset_idx++) {
				if (!fseek(ifp, -offsets[offset_idx], SEEK_END) &&
					fread(head, 1, 32, ifp) && !strncmp(head, "BRCM", 4)) {
					fseek(ifp, -32, SEEK_CUR);
					strcpy(make, "OmniVision");
					width = raw_width;
					//Defaults
					raw_width = 2611;
					filters = 0x16161616;
					parse_raspberrypi();
					break;
				}
			}
	  }
	}// else is_raw = 0;
#else
    fseek(ifp, 0, SEEK_END);
    int sz = ftell(ifp);
    if (!strncmp(model, "RP_imx219", 9) && sz >= 0x9cb600 &&
        !fseek(ifp, -0x9cb600, SEEK_END) && fread(head, 1, 0x20, ifp) &&
        !strncmp(head, "BRCM", 4))
    {
      strcpy(make, "Broadcom");
      strcpy(model, "RPi IMX219");
      if (raw_height > raw_width)
        flip = 5;
      data_offset = ftell(ifp) + 0x8000 - 0x20;
      parse_broadcom();
      black = 66;
      maximum = 0x3ff;
      load_raw = &LibRaw::broadcom_load_raw;
      thumb_offset = 0;
      thumb_length = sz - 0x9cb600 - 1;
    }
    else if (!(strncmp(model, "ov5647", 6) && strncmp(model, "RP_OV5647", 9)) &&
             sz >= 0x61b800 && !fseek(ifp, -0x61b800, SEEK_END) &&
             fread(head, 1, 0x20, ifp) && !strncmp(head, "BRCM", 4))
    {
      strcpy(make, "Broadcom");
      if (!strncmp(model, "ov5647", 6))
        strcpy(model, "RPi OV5647 v.1");
      else
        strcpy(model, "RPi OV5647 v.2");
      if (raw_height > raw_width)
        flip = 5;
      data_offset = ftell(ifp) + 0x8000 - 0x20;
      parse_broadcom();
      black = 16;
      maximum = 0x3ff;
      load_raw = &LibRaw::broadcom_load_raw;
      thumb_offset = 0;
      thumb_length = sz - 0x61b800 - 1;
    }
    else
      is_raw = 0;
#endif
  }

  // make sure strings are terminated
  desc[511] = artist[63] = make[63] = model[63] = model2[63] = 0;

  for (i = 0; i < int(sizeof CorpTable / sizeof *CorpTable); i++)
  {
    if (strcasestr(make, CorpTable[i].CorpName))
    { /* Simplify company names */
      maker_index = CorpTable[i].CorpId;
      strcpy(make, CorpTable[i].CorpName);
    }
  }

  if ((makeIs(LIBRAW_CAMERAMAKER_Kodak) || makeIs(LIBRAW_CAMERAMAKER_Leica)) &&
      ((cp = strcasestr(model, " DIGITAL CAMERA")) ||
       (cp = strstr(model, "FILE VERSION")))) {
    *cp = 0;
  } else if (makeIs(LIBRAW_CAMERAMAKER_Ricoh) && !strncasecmp(model, "PENTAX", 6)) {
    maker_index = LIBRAW_CAMERAMAKER_Pentax;
    strcpy(make, "Pentax");
  } else if (makeIs(LIBRAW_CAMERAMAKER_JK_Imaging) && !strncasecmp(model, "Kodak", 5)) {
    maker_index = LIBRAW_CAMERAMAKER_Kodak;
    strcpy(make, "Kodak");
  }

  remove_trailing_spaces(make, sizeof(make));
  remove_trailing_spaces(model, sizeof(model));

  i = strbuflen(make); /* Remove make from model */
  if (!strncasecmp(model, make, i) && model[i++] == ' ')
    memmove(model, model + i, 64 - i);

  if (makeIs(LIBRAW_CAMERAMAKER_Fujifilm) && !strncmp(model, "FinePix", 7)) {
    memmove(model, model + 7, strlen(model) - 6);
    if (model[0] == ' ') {
      memmove(model, model + 1, strlen(model));
    }
  } else if ((makeIs(LIBRAW_CAMERAMAKER_Kodak) || makeIs(LIBRAW_CAMERAMAKER_Konica)) &&
           !strncmp(model, "Digital Camera ", 15)) {
    memmove(model, model + 15, strlen(model) - 14);
  }

  desc[511] = artist[63] = make[63] = model[63] = model2[63] = 0;
  if (!is_raw)
    goto notraw;

  if (!height)
    height = raw_height;
  if (!width)
    width = raw_width;

  identify_finetune_pentax();


  if (dng_version)
  {
    if (filters == UINT_MAX)
      filters = 0;
    if (!filters)
      colors = tiff_samples;
    switch (tiff_compress)
    {
    case 0: // Compression not set, assuming uncompressed
    case 1:
#ifdef USE_DNGSDK
      // Uncompressed float
      if (load_raw != &LibRaw::float_dng_load_raw_placeholder)
#endif
        load_raw = &LibRaw::packed_dng_load_raw;
      break;
    case 7:
      load_raw = &LibRaw::lossless_dng_load_raw;
      break;
    case 8:
      load_raw = &LibRaw::deflate_dng_load_raw;
      break;
#ifdef USE_GPRSDK
    case 9:
        load_raw = &LibRaw::vc5_dng_load_raw_placeholder;
        break;
#endif
    case 34892:
      load_raw = &LibRaw::lossy_dng_load_raw;
      break;
    default:
      load_raw = 0;
    }
    GetNormalizedModel();
    if (makeIs(LIBRAW_CAMERAMAKER_Olympus) &&
        (OlyID == OlyID_STYLUS_1) && // don't use normalized_model below, it is 'Stylus 1'
        (strchr(model+6, 's') ||
         strchr(model+6, 'S')))
    {
      width -= 16;
    }
    goto dng_skip;
  }

  if (makeIs(LIBRAW_CAMERAMAKER_Canon) && !fsize && tiff_bps != 15)
  {
      bool fromtable = false;
    if (!load_raw)
      load_raw = &LibRaw::lossless_jpeg_load_raw;
    for (i = 0; i < int(sizeof canon / sizeof *canon); i++)
      if (raw_width == canon[i][0] && raw_height == canon[i][1])
      {
        width = raw_width - (left_margin = canon[i][2]);
        height = raw_height - (top_margin = canon[i][3]);
        width -= canon[i][4];
        height -= canon[i][5];
        mask[0][1] = canon[i][6];
        mask[0][3] = -canon[i][7];
        mask[1][1] = canon[i][8];
        mask[1][3] = -canon[i][9];
        if (canon[i][10])
          filters = canon[i][10] * 0x01010101U;
        fromtable = true;
      }
    if ((unique_id | 0x20000ULL) ==
        0x2720000ULL) // "PowerShot G11", "PowerShot S90": 0x2700000, 0x2720000
                      // possibly "PowerShot SX120 IS" (if not chdk hack?): 0x2710000
    {
      left_margin = 8;
      top_margin = 16;
    }
    if(!fromtable && imgdata.makernotes.canon.AverageBlackLevel) // not known, but metadata known
    {
        FORC4 cblack[c] = imgdata.makernotes.canon.ChannelBlackLevel[c];
        black = cblack[4] = cblack[5] = 0;
        // Prevent automatic BL calculation
        mask[0][3] = 1;
        mask[0][1] = 2;

        if(imgdata.makernotes.canon.SensorWidth == raw_width
            && imgdata.makernotes.canon.SensorHeight == raw_height)
        {
            left_margin = (imgdata.makernotes.canon.SensorLeftBorder+1) & 0xfffe; // round to 2
            width = imgdata.makernotes.canon.SensorRightBorder - left_margin;
            top_margin = (imgdata.makernotes.canon.SensorTopBorder +1)  & 0xfffe;
            height = imgdata.makernotes.canon.SensorBottomBorder - top_margin;
        }
    }
  }

  identify_finetune_by_filesize(fsize);

  if (!strcmp(model, "KAI-0340") && find_green(16, 16, 3840, 5120) < 25)
  {
    height = 480;
    top_margin = filters = 0;
    strcpy(model, "C603");
  }

  GetNormalizedModel();

  identify_finetune_dcr(head, fsize, flen);

  /* Early reject for damaged images */
  if (!load_raw || height < 22 || width < 22 ||
      (tiff_bps > 16 &&
       (load_raw != &LibRaw::deflate_dng_load_raw &&
        load_raw != &LibRaw::float_dng_load_raw_placeholder)) ||
      tiff_samples > 4 || colors > 4 ||
      colors < 1
      /* alloc in unpack() may be fooled by size adjust */
      || ((int)width + (int)left_margin > 65535) ||
      ((int)height + (int)top_margin > 65535))
  {
    is_raw = 0;
    RUN_CALLBACK(LIBRAW_PROGRESS_IDENTIFY, 1, 2);
    return;
  }
  if (!model[0])
  {
    sprintf(model, "%dx%d", width, height);
    strcpy(normalized_model, model);
  }

  if (!(imgdata.params.raw_processing_options &
        LIBRAW_PROCESSING_ZEROFILTERS_FOR_MONOCHROMETIFFS) &&
      (filters == UINT_MAX)) // Default dcraw behaviour
    filters = 0x94949494;
  else if (filters == UINT_MAX)
  {
    if (tiff_nifds > 0 && tiff_samples == 1)
    {
      colors = 1;
      filters = 0;
    }
    else
      filters = 0x94949494;
  }

  if (thumb_offset && !thumb_height)
  {
    fseek(ifp, thumb_offset, SEEK_SET);
    if (ljpeg_start(&jh, 1))
    {
      thumb_width = jh.wide;
      thumb_height = jh.high;
    }
  }

dng_skip:
  if (dng_version)
	  identify_process_dng_fields();

  /* Early reject for damaged images again (after dng fields processing) */
  if (!load_raw || height < 22 || width < 22 ||
      (tiff_bps > 16 &&
       (load_raw != &LibRaw::deflate_dng_load_raw &&
        load_raw != &LibRaw::float_dng_load_raw_placeholder)) ||
      tiff_samples > 4 || colors > 4 || colors < 1)
  {
    is_raw = 0;
    RUN_CALLBACK(LIBRAW_PROGRESS_IDENTIFY, 1, 2);
    return;
  }
  {
    // Check cam_mul range
    int cmul_ok = 1;
    FORCC if (cam_mul[c] <= 0.001f) cmul_ok = 0;
    ;

    if (cmul_ok)
    {
      double cmin = cam_mul[0], cmax;
      double cnorm[4];
      FORCC cmin = MIN(cmin, cam_mul[c]);
      FORCC cnorm[c] = cam_mul[c] / cmin;
      cmax = cmin = cnorm[0];
      FORCC
      {
        cmin = MIN(cmin, cnorm[c]);
        cmax = MIN(cmax, cnorm[c]);
      }
      if (cmin <= 0.01f || cmax > 100.f)
        cmul_ok = false;
    }
    if (!cmul_ok)
    {
      if (cam_mul[0] > 0)
        cam_mul[0] = 0;
      cam_mul[3] = 0;
    }
  }
  if ((use_camera_matrix & (((use_camera_wb || dng_version)?0:1) | 0x2)) &&
      cmatrix[0][0] > 0.125)
  {
    memcpy(rgb_cam, cmatrix, sizeof cmatrix);
    raw_color = 0;
  }
  if (raw_color && !CM_found)
    CM_found = adobe_coeff(maker_index, normalized_model);
  else if ((imgdata.color.cam_xyz[0][0] < 0.01) && !CM_found)
    CM_found = adobe_coeff(maker_index, normalized_model, 1);

  if (load_raw == &LibRaw::kodak_radc_load_raw)
    if ((raw_color) && !CM_found)
		CM_found = adobe_coeff(LIBRAW_CAMERAMAKER_Apple, "Quicktake");

  if ((maker_index != LIBRAW_CAMERAMAKER_Unknown) && normalized_model[0])
    SetStandardIlluminants (maker_index, normalized_model);

  // Clear erorneus fuji_width if not set through parse_fuji or for DNG
  if (fuji_width && !dng_version &&
      !(imgdata.process_warnings & LIBRAW_WARN_PARSEFUJI_PROCESSED))
    fuji_width = 0;

  if (fuji_width)
  {
    fuji_width = width >> !fuji_layout;
    filters = fuji_width & 1 ? 0x94949494 : 0x49494949;
    width = (height >> fuji_layout) + fuji_width;
    height = width - 1;
    pixel_aspect = 1;
  }
  else
  {
    if (raw_height < height)
      raw_height = height;
    if (raw_width < width)
      raw_width = width;
  }
  if (!tiff_bps)
    tiff_bps = 12;
  if (!maximum)
  {
    maximum = (1 << tiff_bps) - 1;
    if (maximum < 0x10000 && curve[maximum] > 0 &&
        load_raw == &LibRaw::sony_arw2_load_raw)
      maximum = curve[maximum];
  }
  if (maximum > 0xffff)
    maximum = 0xffff;
  if (!load_raw || height < 22 || width < 22 ||
      (tiff_bps > 16 &&
       (load_raw != &LibRaw::deflate_dng_load_raw &&
        load_raw != &LibRaw::float_dng_load_raw_placeholder)) ||
      tiff_samples > 6 || colors > 4)
    is_raw = 0;

  if (raw_width < 22 || raw_width > 64000 || raw_height < 22 ||
      pixel_aspect < 0.1 || pixel_aspect > 10. ||
      raw_height > 64000)
    is_raw = 0;

#ifdef NO_JASPER
  if (load_raw == &LibRaw::redcine_load_raw)
  {
    is_raw = 0;
    imgdata.process_warnings |= LIBRAW_WARN_NO_JASPER;
  }
#endif
#ifdef NO_JPEG
  if (load_raw == &LibRaw::kodak_jpeg_load_raw ||
      load_raw == &LibRaw::lossy_dng_load_raw)
  {
    is_raw = 0;
    imgdata.process_warnings |= LIBRAW_WARN_NO_JPEGLIB;
  }
#endif
  if (!cdesc[0])
    strcpy(cdesc, colors == 3 ? "RGBG" : "GMCY");
  if (!raw_height)
    raw_height = height;
  if (!raw_width)
    raw_width = width;
  if (filters > 999 && colors == 3)
    filters |= ((filters >> 2 & 0x22222222) | (filters << 2 & 0x88888888)) &
               filters << 1;
notraw:
  if (flip == (int)UINT_MAX)
    flip = tiff_flip;
  if (flip == (int)UINT_MAX)
    flip = 0;

  // Convert from degrees to bit-field if needed
  if (flip > 89 || flip < -89)
  {
    switch ((flip + 3600) % 360)
    {
    case 270:
      flip = 5;
      break;
    case 180:
      flip = 3;
      break;
    case 90:
      flip = 6;
      break;
    }
  }

  if (pana_bpp)
    imgdata.color.raw_bps = pana_bpp;
  else if ((load_raw == &LibRaw::phase_one_load_raw) ||
           (load_raw == &LibRaw::phase_one_load_raw_c))
    imgdata.color.raw_bps = ph1.format;
  else
    imgdata.color.raw_bps = tiff_bps;

  RUN_CALLBACK(LIBRAW_PROGRESS_IDENTIFY, 1, 2);
}