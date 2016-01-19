#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <QtGlobal>

/* Empty message ID.                       */
#define TELEMETRY_MSG_NOMSG         0x00
/* Telemetry message header size in bytes. */
#define TELEMETRY_MSG_HDR_SIZE      0x02
/* Telemetry buffer size in bytes.         */
#define TELEMETRY_MSG_BUFFER_SIZE   0x20
/* Telemetry data_size member offset.      */
#define TELEMETRY_MSG_DATA_SIZE_ID  0x01

typedef struct tagTelemetryMessage {
    quint8 msg_id;     /* Telemetry message ID.           */
    quint8 data_size;  /* Size of telemetry message data. */
    char data[TELEMETRY_MSG_BUFFER_SIZE]; /* Data buffer. */
} __attribute__((packed)) TelemetryMessage, *PTelemetryMessage;

Q_DECLARE_METATYPE(TelemetryMessage);

#endif // TELEMETRY_H
