#include "pxt.h"
//#include "CodalFiber.h"

#include "comm/src/coproc_codec.h"
#include "comm/src/coproc_link_parser.h"
#include "comm/src/rbcx.pb.h"

#include "Serial.h"

using namespace pxt;

static rb::CoprocCodec codec;
static rb::CoprocLinkParser<CoprocReq, &CoprocReq_msg> parser(codec);

static uint8_t encodingBuf[rb::CoprocCodec::MaxFrameSize];

namespace rbcx
{

    //%
    void foo()
    {
        uBit.display.image.print('c');

        CoprocReq msg = {};
        msg.which_payload = CoprocReq_versionReq_tag;

        auto encodedLen = codec.encodeWithHeader(&CoprocReq_msg, &msg, encodingBuf, sizeof(encodingBuf));

        uBit.serial.send(encodingBuf, encodedLen);
    }

    static void sendUtsResponse()
    {
        CoprocStat msg = {};
        msg.which_payload = CoprocStat_ultrasoundStat_tag;
        msg.payload.ultrasoundStat.utsIndex = 1;
        msg.payload.ultrasoundStat.roundtripMicrosecs = 4444;

        auto encodedLen = codec.encodeWithHeader(&CoprocStat_msg, &msg, encodingBuf, sizeof(encodingBuf));

        uBit.serial.send(encodingBuf, encodedLen);
    }

    static void rxReader()
    {
        uint8_t buf[32];

        uBit.display.image.print('d');

        while (true)
        {
            // DEVICE_ID_SERIAL CODAL_SERIAL_EVT_DATA_RECEIVED
            // fiber_wait_for_event(12, 4);

            int len = uBit.serial.read(buf, sizeof(buf), ASYNC);
            if (len <= 0)
            {
                fiber_sleep(1);
                continue;
            }

            uBit.display.image.print('e');

            for (int i = 0; i < len; ++i)
            {
                if (parser.add(buf[i]))
                {
                    const auto &msg = parser.lastMessage();
                    uBit.display.image.print('0' + msg.which_payload);

                    if (msg.which_payload == CoprocReq_ultrasoundReq_tag)
                    {
                        sendUtsResponse();
                    }
                }
            }
        }
    }

    //%
    void initialize()
    {
#if MICROBIT_CODAL
        // uBit.serial.redirect(uBit.io.P0, uBit.io.P1);
        uBit.serial.setBaudrate(921600);
#else
        // uBit.serial.redirect(uBit.io.P0.name, uBit.io.P1.name);
        uBit.serial.baud(921600);
#endif

        create_fiber(rxReader);

        uBit.display.image.print('c');
    }
};
