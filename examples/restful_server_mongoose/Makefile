CFLAGS += -DCS_PLATFORM=3 \
           -std=gnu99 \
           -ffunction-sections \
           -fdata-sections \
           -DMG_NO_BSD_SOCKETS \
           -DMG_DISABLE_FILESYSTEM \
           -DRTOS_SDK -DMG_LWIP -DLWIP_TIMEVAL_PRIVATE=0 \
           -DMG_INTERNAL=

PROGRAM=http_get_mongoose
include ../../common.mk
