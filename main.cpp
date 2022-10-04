#include "pxt.h"

#include "comm/src/coproc_codec.h"
#include "comm/src/rbcx.pb.h"

using namespace pxt;

namespace rbcx
{

    //%
    int foo()
    {
        uBit.display.image.print('c');
        rb::CoprocCodec c;
        CoprocReq msg = {};
        msg.which_payload = CoprocReq_versionReq_tag;

        uint8_t buf[rb::CoprocCodec::MaxFrameSize];
        auto encodedLen = c.encodeWithHeader(&CoprocReq_msg, &msg, buf, sizeof(buf));
        uBit.serial.send(buf, encodedLen);
        return 4;
    }
};
