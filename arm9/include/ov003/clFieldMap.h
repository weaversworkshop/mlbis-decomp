#ifndef OV003_CL_FIELD_MAP_H
#define OV003_CL_FIELD_MAP_H

#include "types.h"

#ifdef __cplusplus

#include "main/task.h"
#include "main/clVCountTask.h"

class clFieldMap : public clTask {
public:
    u8 pad_18[0x10];
    clVCountTask * field28;
    u8 pad_2c[0x72c];
    u16 field758;
    u16 field75a;

    virtual ~clFieldMap();  // D1 0x0206bf64 D0 0x0206c198
    virtual void run(void);  // 0x0206c3d4
    virtual void func_ov003_0206e694(void);  // 0x0206e694
    virtual void func_ov003_0206ebf0(void);  // 0x0206ebf0
    virtual void func_ov003_0206ecec(void);  // 0x0206ecec
    virtual void func_ov003_0206ed00(void);  // 0x0206ed00
    virtual void func_ov003_0206f164(void);  // 0x0206f164
    virtual void func_ov003_0206f884(void);  // 0x0206f884
    virtual void func_ov003_0206f890(void);  // 0x0206f890
    virtual void func_ov003_0206fa44(void);  // 0x0206fa44
    virtual void func_ov003_0206fc80(void);  // 0x0206fc80
    virtual void func_ov003_0206fca8(void);  // 0x0206fca8
    virtual void func_ov003_0206fce0(void);  // 0x0206fce0
    virtual void func_ov003_0206fdc8(void);  // 0x0206fdc8
    virtual void func_ov003_0206fe00(void);  // 0x0206fe00
    virtual void func_ov003_0206ff10(void);  // 0x0206ff10
    virtual void func_ov003_020735cc(void);  // 0x020735cc
    virtual void func_ov003_0206ff50(void);  // 0x0206ff50
    virtual void func_ov003_0206ff64(void);  // 0x0206ff64
    virtual void func_ov003_0206ff98(void);  // 0x0206ff98
    virtual void func_ov003_0206ffb0(void);  // 0x0206ffb0
    virtual void func_ov003_0206ffd0(void);  // 0x0206ffd0
    virtual void func_ov003_0206ffd8(void);  // 0x0206ffd8
    virtual void func_ov003_0206ffec(void);  // 0x0206ffec
    virtual void func_ov003_02070000(void);  // 0x02070000
    virtual void func_ov003_02070014(void);  // 0x02070014
    virtual void func_ov003_0207001c(void);  // 0x0207001c
    virtual void func_ov003_020735c0(void);  // 0x020735c0
    virtual void func_ov003_020735b4(void);  // 0x020735b4
    virtual void func_ov003_020735a8(void);  // 0x020735a8
    virtual void func_ov003_0207359c(void);  // 0x0207359c
    virtual void func_ov003_02073574(void);  // 0x02073574
    virtual void func_ov003_02070030(void);  // 0x02070030
    virtual void func_ov003_02070334(void);  // 0x02070334
    virtual void func_ov003_020704f0(void);  // 0x020704f0
    virtual void func_ov003_0207356c(void);  // 0x0207356c
    virtual void func_ov003_02070504(void);  // 0x02070504
    virtual void func_ov003_02070554(void);  // 0x02070554
    virtual void func_ov003_02070730(void);  // 0x02070730
    virtual void func_ov003_020709a0(void);  // 0x020709a0
    virtual void func_ov003_020709d0(void);  // 0x020709d0
    virtual void func_ov003_02073564(void);  // 0x02073564
    virtual void func_ov003_02073548(void);  // 0x02073548
    virtual void func_ov003_0206e630(void);  // 0x0206e630
    virtual void func_ov003_020734e4(void);  // 0x020734e4
    virtual void func_ov003_02070a0c(void);  // 0x02070a0c
    virtual void func_ov003_02070a64(void);  // 0x02070a64
    virtual void func_ov003_02070dbc(void);  // 0x02070dbc
    virtual void func_ov003_02070b04(void);  // 0x02070b04
    virtual void func_ov003_02070ec8(void);  // 0x02070ec8
    virtual void func_ov003_02070edc(void);  // 0x02070edc
    virtual void func_ov003_02070f6c(void);  // 0x02070f6c
    virtual void func_ov003_02070fa0(void);  // 0x02070fa0
    virtual void func_ov003_02071034(void);  // 0x02071034
    virtual void func_ov003_02071054(void);  // 0x02071054
    virtual void func_ov003_02071064(void);  // 0x02071064
    virtual void func_ov003_020710a8(void);  // 0x020710a8
    virtual void func_ov003_020734dc(void);  // 0x020734dc
    virtual void func_ov003_020711a4(void);  // 0x020711a4
    virtual void func_ov003_020712a0(void);  // 0x020712a0
    virtual void func_ov003_020715fc(void);  // 0x020715fc
    virtual void func_ov003_020734d0(void);  // 0x020734d0
    virtual void func_ov003_020734b4(void);  // 0x020734b4
    virtual void func_ov003_020734a0(void);  // 0x020734a0
    virtual void func_ov003_02073484(void);  // 0x02073484
    virtual void func_ov003_02073470(void);  // 0x02073470
    virtual void func_ov003_02071734(void);  // 0x02071734
    virtual void func_ov003_020718d8(void);  // 0x020718d8
    virtual void func_ov003_02071a10(void);  // 0x02071a10
    virtual void func_ov003_02071a68(void);  // 0x02071a68
    virtual void func_ov003_02071a90(void);  // 0x02071a90
    virtual void func_ov003_02071ac4(void);  // 0x02071ac4
    virtual void func_ov003_02071bcc(void);  // 0x02071bcc
    virtual void func_ov003_02073464(void);  // 0x02073464
    virtual u16 func_ov003_0206aec8(void) { return this->field758; }  // 0x0206aec8
    virtual u16 func_ov003_0206aed4(void) { return this->field75a; }  // 0x0206aed4
    virtual void func_ov003_02073418(void);  // 0x02073418
    virtual void func_ov003_0206ebb8(void);  // 0x0206ebb8
    virtual void func_ov003_02072b44(void);  // 0x02072b44
    virtual void func_ov003_02073350(void);  // 0x02073350
    virtual void func_ov003_02073334(void);  // 0x02073334
    virtual void func_ov003_02073320(void);  // 0x02073320
    virtual void func_ov003_02071bf8(void);  // 0x02071bf8
    virtual void func_ov003_02071c48(void);  // 0x02071c48
    virtual void func_ov003_02071ccc(void);  // 0x02071ccc
    virtual void func_ov003_02071d18(void);  // 0x02071d18
    virtual void func_ov003_020720f0(void);  // 0x020720f0
    virtual void func_ov003_02072120(void);  // 0x02072120
    virtual void func_ov003_02072134(void);  // 0x02072134
    virtual void func_ov003_0207217c(void);  // 0x0207217c
    virtual void func_ov003_020721c8(void);  // 0x020721c8
    virtual void func_ov003_02070718(void);  // 0x02070718
    virtual void func_ov003_020721dc(void);  // 0x020721dc
};

#endif

#endif
