// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0200907c.h"

#include <nnsys/snd.h>

// MATCH 100%
void func_0200907c() {
    UnkStruct_01ff94c4 * r7 = data_01ff94c4;
    s8 r8;
    NNSSndHandle * r9 = &data_01ff94a8;
    s16 r10 = 1;
    u32 r11 = 0;

    while(r10 >= 0) {
        if (r7->field0c != 0 && r7->field0c != 3) {

            u32 flag = (data_01ff94a4[r10].player != NULL);
            if (flag != 0) {
                switch (r7->field0c) {
                    case 1:
                        r7->field04 += r7->field08;

                        if (r7->field04 > r7->field00) {
                            r7->field04 = r7->field00;
                            r7->field0c = r11;
                        }

                        NNS_SndPlayerSetTrackMuteEx(r9, 0xffff, 0);
                        break;
                    case 2:
                        r7->field04 -= r7->field08;
                        if (r7->field04 < 0) {
                            r7->field04 = 0;
                            r7->field0c = 3;
                            NNS_SndPlayerSetTrackMuteEx(r9, 0xffff, 1);
                        }
                        break;
                    default:
                        break;
                }

                if (r7->field04 != 0) {
                    NNS_SndPlayerSetTrackVolume(r9, 0xffff, 0xfff & (r7->field04 >> 0xc));
                }

                const NNSSndArcSeqInfo * ret = NNS_SndArcGetSeqInfo(data_01ff94a4[r10].player->seqNo);
                s8 r1;

                if (r7->field04 == r7->field00) {
                    r1 = ret->param.playerPrio;
                    r8 = ret->param.channelPrio;
                } else {
                    switch (r7->field0c) {
                        case 1:
                            r1 = 1;
                            r8 = r1;
                            break;
                        case 2:
                        default:
                            r1 = 0;
                            r8 = r1;
                    }
                }

                NNS_SndPlayerSetPlayerPriority(r9, r1);
                NNS_SndPlayerSetChannelPriority(r9, r8);
            }
        }

        r9--;
        r10--;
        r7--;
    }
}
