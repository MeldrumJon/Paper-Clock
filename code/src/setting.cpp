#include "setting.h"

static setting_meridiem_t meridiem_setting = AM;

void setting_setMeridiem(setting_meridiem_t meridiem) {
    meridiem_setting = meridiem;
}

setting_meridiem_t setting_getMeridiem() {
    return meridiem_setting;
}

void setting_meridiemNext(uint8_t backwards) {
    switch (meridiem_setting) {
        case AM:
            meridiem_setting = (!backwards) ? PM : MILITARY;
            break;
        case PM:
            meridiem_setting = (!backwards) ? MILITARY : AM;
            break;
        case MILITARY:
            meridiem_setting = (!backwards) ? AM : PM;
            break;
    }
    return;
}
