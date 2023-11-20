#include <TcpSerialBridge2.h>
#include <FullLoopbackStream.h>
#include <Arduino_GFX_Library.h>

TcpSerialBridge2 instance(BRIDGE_PORT);

class ECrowneWifi {
    public:
        static void setup(FullLoopbackStream *outgoingStream, FullLoopbackStream *incomingStream, Arduino_GFX *gfx) {
            instance.setup(outgoingStream, incomingStream, gfx);
        }
        static void loop() {
            instance.loop();
        }
        static void flush() {
            instance.flush();
        }
};

// these will override the Serial interface that SimHub uses to use our Streams
#define FlowSerialBegin [](unsigned long baud) {}
#define StreamRead incomingStream.read
#define StreamAvailable incomingStream.available
#define FlowSerialFlush ECrowneWifi::flush
#define StreamFlush ECrowneWifi::flush
#define StreamWrite outgoingStream.write
#define StreamPrint outgoingStream.print