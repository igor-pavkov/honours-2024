Bool TY_(IsXMLLetter)(uint c)
{
    return ((c >= 0x41 && c <= 0x5a) ||
        (c >= 0x61 && c <= 0x7a) ||
        (c >= 0xc0 && c <= 0xd6) ||
        (c >= 0xd8 && c <= 0xf6) ||
        (c >= 0xf8 && c <= 0xff) ||
        (c >= 0x100 && c <= 0x131) ||
        (c >= 0x134 && c <= 0x13e) ||
        (c >= 0x141 && c <= 0x148) ||
        (c >= 0x14a && c <= 0x17e) ||
        (c >= 0x180 && c <= 0x1c3) ||
        (c >= 0x1cd && c <= 0x1f0) ||
        (c >= 0x1f4 && c <= 0x1f5) ||
        (c >= 0x1fa && c <= 0x217) ||
        (c >= 0x250 && c <= 0x2a8) ||
        (c >= 0x2bb && c <= 0x2c1) ||
        c == 0x386 ||
        (c >= 0x388 && c <= 0x38a) ||
        c == 0x38c ||
        (c >= 0x38e && c <= 0x3a1) ||
        (c >= 0x3a3 && c <= 0x3ce) ||
        (c >= 0x3d0 && c <= 0x3d6) ||
        c == 0x3da ||
        c == 0x3dc ||
        c == 0x3de ||
        c == 0x3e0 ||
        (c >= 0x3e2 && c <= 0x3f3) ||
        (c >= 0x401 && c <= 0x40c) ||
        (c >= 0x40e && c <= 0x44f) ||
        (c >= 0x451 && c <= 0x45c) ||
        (c >= 0x45e && c <= 0x481) ||
        (c >= 0x490 && c <= 0x4c4) ||
        (c >= 0x4c7 && c <= 0x4c8) ||
        (c >= 0x4cb && c <= 0x4cc) ||
        (c >= 0x4d0 && c <= 0x4eb) ||
        (c >= 0x4ee && c <= 0x4f5) ||
        (c >= 0x4f8 && c <= 0x4f9) ||
        (c >= 0x531 && c <= 0x556) ||
        c == 0x559 ||
        (c >= 0x561 && c <= 0x586) ||
        (c >= 0x5d0 && c <= 0x5ea) ||
        (c >= 0x5f0 && c <= 0x5f2) ||
        (c >= 0x621 && c <= 0x63a) ||
        (c >= 0x641 && c <= 0x64a) ||
        (c >= 0x671 && c <= 0x6b7) ||
        (c >= 0x6ba && c <= 0x6be) ||
        (c >= 0x6c0 && c <= 0x6ce) ||
        (c >= 0x6d0 && c <= 0x6d3) ||
        c == 0x6d5 ||
        (c >= 0x6e5 && c <= 0x6e6) ||
        (c >= 0x905 && c <= 0x939) ||
        c == 0x93d ||
        (c >= 0x958 && c <= 0x961) ||
        (c >= 0x985 && c <= 0x98c) ||
        (c >= 0x98f && c <= 0x990) ||
        (c >= 0x993 && c <= 0x9a8) ||
        (c >= 0x9aa && c <= 0x9b0) ||
        c == 0x9b2 ||
        (c >= 0x9b6 && c <= 0x9b9) ||
        (c >= 0x9dc && c <= 0x9dd) ||
        (c >= 0x9df && c <= 0x9e1) ||
        (c >= 0x9f0 && c <= 0x9f1) ||
        (c >= 0xa05 && c <= 0xa0a) ||
        (c >= 0xa0f && c <= 0xa10) ||
        (c >= 0xa13 && c <= 0xa28) ||
        (c >= 0xa2a && c <= 0xa30) ||
        (c >= 0xa32 && c <= 0xa33) ||
        (c >= 0xa35 && c <= 0xa36) ||
        (c >= 0xa38 && c <= 0xa39) ||
        (c >= 0xa59 && c <= 0xa5c) ||
        c == 0xa5e ||
        (c >= 0xa72 && c <= 0xa74) ||
        (c >= 0xa85 && c <= 0xa8b) ||
        c == 0xa8d ||
        (c >= 0xa8f && c <= 0xa91) ||
        (c >= 0xa93 && c <= 0xaa8) ||
        (c >= 0xaaa && c <= 0xab0) ||
        (c >= 0xab2 && c <= 0xab3) ||
        (c >= 0xab5 && c <= 0xab9) ||
        c == 0xabd ||
        c == 0xae0 ||
        (c >= 0xb05 && c <= 0xb0c) ||
        (c >= 0xb0f && c <= 0xb10) ||
        (c >= 0xb13 && c <= 0xb28) ||
        (c >= 0xb2a && c <= 0xb30) ||
        (c >= 0xb32 && c <= 0xb33) ||
        (c >= 0xb36 && c <= 0xb39) ||
        c == 0xb3d ||
        (c >= 0xb5c && c <= 0xb5d) ||
        (c >= 0xb5f && c <= 0xb61) ||
        (c >= 0xb85 && c <= 0xb8a) ||
        (c >= 0xb8e && c <= 0xb90) ||
        (c >= 0xb92 && c <= 0xb95) ||
        (c >= 0xb99 && c <= 0xb9a) ||
        c == 0xb9c ||
        (c >= 0xb9e && c <= 0xb9f) ||
        (c >= 0xba3 && c <= 0xba4) ||
        (c >= 0xba8 && c <= 0xbaa) ||
        (c >= 0xbae && c <= 0xbb5) ||
        (c >= 0xbb7 && c <= 0xbb9) ||
        (c >= 0xc05 && c <= 0xc0c) ||
        (c >= 0xc0e && c <= 0xc10) ||
        (c >= 0xc12 && c <= 0xc28) ||
        (c >= 0xc2a && c <= 0xc33) ||
        (c >= 0xc35 && c <= 0xc39) ||
        (c >= 0xc60 && c <= 0xc61) ||
        (c >= 0xc85 && c <= 0xc8c) ||
        (c >= 0xc8e && c <= 0xc90) ||
        (c >= 0xc92 && c <= 0xca8) ||
        (c >= 0xcaa && c <= 0xcb3) ||
        (c >= 0xcb5 && c <= 0xcb9) ||
        c == 0xcde ||
        (c >= 0xce0 && c <= 0xce1) ||
        (c >= 0xd05 && c <= 0xd0c) ||
        (c >= 0xd0e && c <= 0xd10) ||
        (c >= 0xd12 && c <= 0xd28) ||
        (c >= 0xd2a && c <= 0xd39) ||
        (c >= 0xd60 && c <= 0xd61) ||
        (c >= 0xe01 && c <= 0xe2e) ||
        c == 0xe30 ||
        (c >= 0xe32 && c <= 0xe33) ||
        (c >= 0xe40 && c <= 0xe45) ||
        (c >= 0xe81 && c <= 0xe82) ||
        c == 0xe84 ||
        (c >= 0xe87 && c <= 0xe88) ||
        c == 0xe8a ||
        c == 0xe8d ||
        (c >= 0xe94 && c <= 0xe97) ||
        (c >= 0xe99 && c <= 0xe9f) ||
        (c >= 0xea1 && c <= 0xea3) ||
        c == 0xea5 ||
        c == 0xea7 ||
        (c >= 0xeaa && c <= 0xeab) ||
        (c >= 0xead && c <= 0xeae) ||
        c == 0xeb0 ||
        (c >= 0xeb2 && c <= 0xeb3) ||
        c == 0xebd ||
        (c >= 0xec0 && c <= 0xec4) ||
        (c >= 0xf40 && c <= 0xf47) ||
        (c >= 0xf49 && c <= 0xf69) ||
        (c >= 0x10a0 && c <= 0x10c5) ||
        (c >= 0x10d0 && c <= 0x10f6) ||
        c == 0x1100 ||
        (c >= 0x1102 && c <= 0x1103) ||
        (c >= 0x1105 && c <= 0x1107) ||
        c == 0x1109 ||
        (c >= 0x110b && c <= 0x110c) ||
        (c >= 0x110e && c <= 0x1112) ||
        c == 0x113c ||
        c == 0x113e ||
        c == 0x1140 ||
        c == 0x114c ||
        c == 0x114e ||
        c == 0x1150 ||
        (c >= 0x1154 && c <= 0x1155) ||
        c == 0x1159 ||
        (c >= 0x115f && c <= 0x1161) ||
        c == 0x1163 ||
        c == 0x1165 ||
        c == 0x1167 ||
        c == 0x1169 ||
        (c >= 0x116d && c <= 0x116e) ||
        (c >= 0x1172 && c <= 0x1173) ||
        c == 0x1175 ||
        c == 0x119e ||
        c == 0x11a8 ||
        c == 0x11ab ||
        (c >= 0x11ae && c <= 0x11af) ||
        (c >= 0x11b7 && c <= 0x11b8) ||
        c == 0x11ba ||
        (c >= 0x11bc && c <= 0x11c2) ||
        c == 0x11eb ||
        c == 0x11f0 ||
        c == 0x11f9 ||
        (c >= 0x1e00 && c <= 0x1e9b) ||
        (c >= 0x1ea0 && c <= 0x1ef9) ||
        (c >= 0x1f00 && c <= 0x1f15) ||
        (c >= 0x1f18 && c <= 0x1f1d) ||
        (c >= 0x1f20 && c <= 0x1f45) ||
        (c >= 0x1f48 && c <= 0x1f4d) ||
        (c >= 0x1f50 && c <= 0x1f57) ||
        c == 0x1f59 ||
        c == 0x1f5b ||
        c == 0x1f5d ||
        (c >= 0x1f5f && c <= 0x1f7d) ||
        (c >= 0x1f80 && c <= 0x1fb4) ||
        (c >= 0x1fb6 && c <= 0x1fbc) ||
        c == 0x1fbe ||
        (c >= 0x1fc2 && c <= 0x1fc4) ||
        (c >= 0x1fc6 && c <= 0x1fcc) ||
        (c >= 0x1fd0 && c <= 0x1fd3) ||
        (c >= 0x1fd6 && c <= 0x1fdb) ||
        (c >= 0x1fe0 && c <= 0x1fec) ||
        (c >= 0x1ff2 && c <= 0x1ff4) ||
        (c >= 0x1ff6 && c <= 0x1ffc) ||
        c == 0x2126 ||
        (c >= 0x212a && c <= 0x212b) ||
        c == 0x212e ||
        (c >= 0x2180 && c <= 0x2182) ||
        (c >= 0x3041 && c <= 0x3094) ||
        (c >= 0x30a1 && c <= 0x30fa) ||
        (c >= 0x3105 && c <= 0x312c) ||
        (c >= 0xac00 && c <= 0xd7a3) ||
        (c >= 0x4e00 && c <= 0x9fa5) ||
        c == 0x3007 ||
        (c >= 0x3021 && c <= 0x3029) ||
        (c >= 0x4e00 && c <= 0x9fa5) ||
        c == 0x3007 ||
        (c >= 0x3021 && c <= 0x3029));
}