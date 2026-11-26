// Copyright (C) 2024 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

// The ExecutionPlatform runtime that establishes a way to call functions at runtime from the test system.

#include <ep/app.h>
#include <string.h>

// Two limiting factors for callback registration.
// Can be defined by user to balance test case demands and static memory consumption.
// Note that ep_register fails with return value false when these limits are exceeded.
#ifndef EP_REGISTER_MAX_CALLS
    #define EP_REGISTER_MAX_CALLS 10         // Max total number of callbacks
#endif

#ifndef EP_REGISTER_MAX_TOTAL_ID_LEN
    #define EP_REGISTER_MAX_TOTAL_ID_LEN 200    // Max combined size of all registered ids
#endif

#define EP_MAX_PAYLOAD 255
#define EP_INTERNAL_RESP_BUFFER_SIZE 50

enum ep_req_code {
    EP_REQ_CODE_UNKNOWN = 0,
    EP_REQ_CODE_CMD_INVOKE = 100,
};

enum ep_app_data_dynamic_code {
    EP_APP_DATA_DYNAMIC_INVOKE_NO_CALLBACK = 10,
    EP_APP_DATA_DYNAMIC_INVOKE_RESP = 20,
};

static int CallbacksUsed = 0;
static Callback CallbackPointers[EP_REGISTER_MAX_CALLS];
static uint8_t CallbackIdLen[EP_REGISTER_MAX_CALLS];
static uint8_t CallbackIds[EP_REGISTER_MAX_TOTAL_ID_LEN]; // Contains all the id strings continously.

struct Message {
    enum ep_req_code id;
    int ExpectedPayloadLength;
    int CurrentPayload;
    uint8_t Payload[EP_MAX_PAYLOAD];
};
static struct Message message;

Callback ep_internal_get_callback(uint8_t const* search_id, int search_id_len) {
    // Iterate through all registered callbacks and check wether one matches the request
    int idbuf_start_pos = 0;

    for (int i=0; i<CallbacksUsed; ++i) {
        int currentStartPos = idbuf_start_pos;
        int len = CallbackIdLen[i];
        idbuf_start_pos += len;

        if (search_id_len != len) { continue; }

        if (memcmp(search_id, &CallbackIds[currentStartPos], search_id_len) == 0) {
            return CallbackPointers[i];
        }
    }
    return NULL;
}

void ep_internal_dynamic_response(enum ep_app_data_dynamic_code code, void const* resp, int len)
{
    // If the BSP uses a cached implementation for stdout, we need
    // to make sure that all stdout is sent before we proceed with
    // the response.
    ep_bsp_flush();

    uint8_t buffer[EP_INTERNAL_RESP_BUFFER_SIZE];
    if (len > (EP_INTERNAL_RESP_BUFFER_SIZE-1)) {
        ep_abort("dynamic_response exceeds buffer length.");
    }
    buffer[0] = (uint8_t)(code);
    memcpy(buffer+1, resp, len);
    ep_internal_resp(EP_RESP_APP_DATA_RAW, buffer, len+1);
}

struct Message* ep_internal_process_next_cmd(void)
{
    message.id = EP_REQ_CODE_UNKNOWN;
    message.ExpectedPayloadLength = -1;
    message.CurrentPayload = 0;
    for (int i=0;i<EP_MAX_PAYLOAD;++i) {
        message.Payload[i] = 0;
    }

    int delim1=-1, delim2=-1;
    for (;;) {
        uint8_t b = ep_bsp_receive_byte();
        if (message.id == EP_REQ_CODE_UNKNOWN) {
            if (b == (uint8_t)EP_REQ_CODE_CMD_INVOKE) {
                message.id = EP_REQ_CODE_CMD_INVOKE;
                continue;
            }
        }
        if (message.ExpectedPayloadLength == -1) {
            message.ExpectedPayloadLength = b;
            continue;
        }
        if (message.CurrentPayload != message.ExpectedPayloadLength) {
            message.Payload[message.CurrentPayload] = b;
            message.CurrentPayload++;
            continue;
        }
        if (delim1 == -1) {
            delim1 = b;
            continue;
        }
        if (delim2 == -1) {
            delim2 = b;

            if (delim1 == 0x3D && delim2 == 0x29) {
                // Valid message. Prepare for external usage.
                // Single buffer sufficient, next message will only be received
                // when processing this one has finished.
                return &message;
            }
            else {
                return NULL;
            }
        }
        return NULL;
    }
}

void ep_internal_resp(enum ep_resp_code code, void const* payload, int payloadLength) {
    ep_bsp_output_bytes((uint8_t*)&code, 1);
    // Currently payload length is limited to 255 bytes.
    uint8_t const limitedPayloadLength = payloadLength;
    ep_bsp_output_bytes(&limitedPayloadLength, 1);

    uint8_t* b = (uint8_t*)payload;
    ep_bsp_output_bytes(b, limitedPayloadLength);

    static const uint8_t Delimiter[] = {0x3D, 0x29};
    ep_bsp_output_bytes(Delimiter, 2);
}

void ep_internal_resp_string(enum ep_resp_code code, char const* payload) {
    ep_internal_resp(code, payload, strlen(payload));
}

void ep_internal_resp_splitted(enum ep_resp_code code, void const* payload, int len) {
    int offset = 0;
    uint8_t const* dataPtr = payload;
    while (offset < len) {
        uint8_t const* frameStart = dataPtr + offset;
        int frameLength = len - offset;
        if (frameLength > EP_MAX_PAYLOAD) {
            frameLength = EP_MAX_PAYLOAD;
        }

        ep_internal_resp(code, frameStart, frameLength);
        offset += frameLength;
    }
}

void ep_internal_resp_splitted_string(enum ep_resp_code code, char const* string) {
    ep_internal_resp_splitted(code, string, strlen(string));
}

void ep_abort(char const* reason) {
    ep_internal_resp(EP_RESP_ABORT, reason, strlen(reason));
    for (;;) {}
}

void ep_register(const char* method_name, Callback function) {
    uint8_t const* rawId = (uint8_t const*)method_name;
    if (ep_internal_get_callback(rawId, strlen(method_name)) != NULL) {
        ep_abort("Registered ID is duplicated");
    }

    int idLen = strlen(method_name);
    if (idLen > 255) {
        ep_abort("Registered ID length is too long");
    }

    if (CallbacksUsed == EP_REGISTER_MAX_CALLS) {
        ep_abort("ep_register exceeded Callback Registration Limit");
    }

    int idBufStart = 0;
    for (int i=0; i<CallbacksUsed; ++i) {
        idBufStart += CallbackIdLen[i];
    }
    if (idBufStart + idLen > EP_REGISTER_MAX_TOTAL_ID_LEN) {
        ep_abort("ep_register exceeded Total ID Limit");
    }

    CallbackPointers[CallbacksUsed] = function;
    CallbackIdLen[CallbacksUsed] = idLen;
    memcpy(CallbackIds+idBufStart, method_name, idLen);
    CallbacksUsed++;
}

int ep_process_loop(void) {
    static char const *handler = "dynamic-invoke";
    ep_internal_resp_string(EP_RESP_APP_LOADED, handler);

    for (;;) {
        struct Message* m = ep_internal_process_next_cmd();

        if (m != NULL) {
            // Structure
            // 1 byte: Length N of method name
            // N bytes: Method name bytes as ascii
            // M bytes: Argument data
            uint8_t function_name_length = m->Payload[0];
            // std::string function_name(m->Data()+1, m->Data()+1+function_name_length);
            uint8_t const* argDataStart = m->Payload+1+function_name_length;
            int argDataLen = m->ExpectedPayloadLength-1-function_name_length;

            Callback c = ep_internal_get_callback(m->Payload+1, function_name_length);
            if (c != NULL) {
                int32_t res = c(argDataStart, argDataLen);
                ep_internal_dynamic_response(EP_APP_DATA_DYNAMIC_INVOKE_RESP, &res, sizeof(int32_t));
            }
            else {
                ep_internal_dynamic_response(EP_APP_DATA_DYNAMIC_INVOKE_NO_CALLBACK, NULL, 0);
            }
        } else {
            ep_abort("ep_invoke received corrupted message.");
        }
    }
    return 0;
}

int ep_main(void) {
    int initRes = ep_bsp_init();

    for (int i=0; i<50;++i) {
        // Send invariant characters to ensure communication integrity
        // The receiving side will discard the first 5 bytes and expect only remaining 0s until communication starts.
        uint8_t const initializer = 0;
        ep_bsp_output_bytes(&initializer, 1);
    }

    if (initRes == 0) {
        static char const* BSP_VERSION = "9.8.0";
        ep_internal_resp_string(EP_RESP_BSP_LOADED, BSP_VERSION);

        ep_app_init();
        ep_app_main(); // This function has to be compiled with C-Linkage!
        ep_abort("ep_app_main returned unexpectedly.");
    }
    return -1;
}

#ifndef EP_CUSTOM_MAIN
// Define EP_CUSTOM_MAIN if your environment defines its own main function.
// Custom main functions must call ep_main() to start the EP runtime.
int main(void) {
    return ep_main();
}
#endif
