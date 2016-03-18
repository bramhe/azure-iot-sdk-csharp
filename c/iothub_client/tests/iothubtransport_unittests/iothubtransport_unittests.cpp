// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif



#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "iothub_client.h"
#include "iothubtransport.h"

#include "lock.h"
#include "doublylinkedlist.h"
#include "vector.h"


#include "string_tokenizer.h"
#include "strings.h"

#include "tickcounter.h"


extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

namespace BASEIMPLEMENTATION
{
#define Lock(x) (LOCK_OK + gballocState - gballocState) /*compiler warning about constant in if condition*/
#define Unlock(x) (LOCK_OK + gballocState - gballocState)
#define Lock_Init() (LOCK_HANDLE)0x42
#define Lock_Deinit(x) (LOCK_OK + gballocState - gballocState)
#include "gballoc.c"
#undef Lock
#undef Unlock
#undef Lock_Init
#undef Lock_Deinit

#include "doublylinkedlist.c"
#include "vector.c"

};

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_BATCHSTATE_RESULT, IOTHUB_BATCHSTATE_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);


static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

//Check ProtocolGateway Configuration.
static bool checkProtocolGatewayHostName;
static bool checkProtocolGatewayIsNull;
static size_t howManyDoWorkCalls = 0;
static size_t doWorkCallCount = 0;
static THREAD_START_FUNC threadFunc;
static void* threadFuncArg;

#define TEST_DEVICE_ID "theidofTheDevice"
#define TEST_DEVICE_KEY "theKeyoftheDevice"
#define TEST_IOTHUBNAME "theNameoftheIotHub"
#define TEST_IOTHUBSUFFIX "theSuffixoftheIotHubHostname"
#define TEST_AUTHORIZATIONKEY "theAuthorizationKey"
#define TEST_LOCK_HANDLE (LOCK_HANDLE)0x4443
#define TEST_THREAD_HANDLE (THREAD_HANDLE)0x4442



#define TEST_HOSTNAME_TOKEN "HostName"
#define TEST_HOSTNAME_VALUE "theNameoftheIotHub.theSuffixoftheIotHubHostname"

#define TEST_DEVICEID_TOKEN "DeviceId"
#define TEST_DEVICEKEY_TOKEN "SharedAccessKey"
#define TEST_PROTOCOL_GATEWAY_HOST_NAME_TOKEN "GatewayHostName"

#define TEST_DEVICEMESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x52
#define TEST_DEVICEMESSAGE_HANDLE_2 (IOTHUB_MESSAGE_HANDLE)0x53
#define TEST_IOTHUB_CLIENT_LL_HANDLE    (IOTHUB_CLIENT_LL_HANDLE)0x4242

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_STRING_TOKENIZER_HANDLE (STRING_TOKENIZER_HANDLE)0x48
static const char* TEST_CHAR = "TestChar";

static const void* provideFAKE(void);

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG =
{
    provideFAKE,            /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,         /* const char* deviceId;                        */
    TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
    TEST_IOTHUBNAME,        /* const char* iotHubName;                      */
    TEST_IOTHUBSUFFIX,      /* const char* iotHubSuffix;                    */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_NULL_protocol =
{
    NULL,                   /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,         /* const char* deviceId;                        */
    TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
    TEST_IOTHUBNAME,        /* const char* iotHubName;                      */
    TEST_IOTHUBSUFFIX,      /* const char* iotHubSuffix;                    */
};

#define FAKE_TRANSPORT_HANDLE (TRANSPORT_LL_HANDLE)0xDEAD

static const IOTHUB_CLIENT_DEVICE_CONFIG TEST_DEVICE_CONFIG =
{
	provideFAKE,
	FAKE_TRANSPORT_HANDLE,
	TEST_DEVICE_ID,
	TEST_DEVICE_KEY
};

static const IOTHUB_CLIENT_DEVICE_CONFIG TEST_DEVICE_CONFIG_null_protocol =
{
	NULL,
	FAKE_TRANSPORT_HANDLE,
	TEST_DEVICE_ID,
	TEST_DEVICE_KEY
};

static const IOTHUB_CLIENT_DEVICE_CONFIG TEST_DEVICE_CONFIG_null_handle =
{
	provideFAKE,
	NULL,
	TEST_DEVICE_ID,
	TEST_DEVICE_KEY
};

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;
static IOTHUB_CLIENT_STATUS currentIotHubClientStatus;

TYPED_MOCK_CLASS(CIotHubTransportMocks, CGlobalMock)
{
public:

    /* DoublyLinkedList mocks */
    MOCK_STATIC_METHOD_1(, void, DList_InitializeListHead, PDLIST_ENTRY, listHead)
        BASEIMPLEMENTATION::DList_InitializeListHead(listHead);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, DList_IsListEmpty, PDLIST_ENTRY, listHead)
        int result2 = BASEIMPLEMENTATION::DList_IsListEmpty(listHead);
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_2(, void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry)
        BASEIMPLEMENTATION::DList_InsertTailList(listHead, listEntry);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend)
        BASEIMPLEMENTATION::DList_AppendTailList(listHead, ListToAppend);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry)
        int result2 = BASEIMPLEMENTATION::DList_RemoveEntryList(listEntry);
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_1(, PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead)
        PDLIST_ENTRY entry = BASEIMPLEMENTATION::DList_RemoveHeadList(listHead);
    MOCK_METHOD_END(PDLIST_ENTRY, entry)

    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
        currentmalloc_call++;
        if (whenShallmalloc_fail>0)
        {
            if (currentmalloc_call == whenShallmalloc_fail)
            {
                result2 = (void*)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
        }
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_2(, void*, gballoc_realloc, void*, ptr, size_t, size)
    MOCK_METHOD_END(void*, BASEIMPLEMENTATION::gballoc_realloc(ptr, size));

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_1(, TRANSPORT_LL_HANDLE, FAKE_IoTHubTransport_Create, const IOTHUBTRANSPORT_CONFIG*, config)
		TRANSPORT_LL_HANDLE result2;
		if (checkProtocolGatewayHostName)
		{
			if (config != NULL && config->upperConfig != NULL && config->upperConfig->protocolGatewayHostName != NULL && strcmp(config->upperConfig->protocolGatewayHostName, TEST_CHAR) == 0)
			{
				result2 = (TRANSPORT_LL_HANDLE)(0x42);
			}
			else
			{
				result2 = NULL;
			}
		}
		else if (checkProtocolGatewayIsNull)
		{
			if (config != NULL && config->upperConfig != NULL && config->upperConfig->protocolGatewayHostName == NULL)
			{
				result2 = (TRANSPORT_LL_HANDLE)(0x42);
			}
			else
			{
				result2 = NULL;
			}
		}
		else
		{
			result2 = (TRANSPORT_LL_HANDLE)(0x42);
		}
    MOCK_METHOD_END(TRANSPORT_LL_HANDLE, result2)

    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_SetOption, TRANSPORT_LL_HANDLE, handle, const char*, optionName, const void*, value)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

    MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Destroy, TRANSPORT_LL_HANDLE, handle)
    MOCK_VOID_METHOD_END()

		MOCK_STATIC_METHOD_5(, IOTHUB_DEVICE_HANDLE, FAKE_IoTHubTransport_Register, TRANSPORT_LL_HANDLE, handle, const char*, deviceId, const char*, deviceKey, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, PDLIST_ENTRY, waitingToSend)
		MOCK_METHOD_END(IOTHUB_DEVICE_HANDLE, (IOTHUB_DEVICE_HANDLE)handle)

		MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Unregister, IOTHUB_DEVICE_HANDLE, handle)
		MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, FAKE_IoTHubTransport_Subscribe, TRANSPORT_LL_HANDLE, handle)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Unsubscribe, TRANSPORT_LL_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, FAKE_IoTHubTransport_DoWork, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus)
        *iotHubClientStatus = currentIotHubClientStatus;
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

    MOCK_STATIC_METHOD_2(, void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, messageCallback, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED);

    MOCK_STATIC_METHOD_1(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, (IOTHUB_MESSAGE_HANDLE)((uintptr_t)iotHubMessageHandle + 1000))

    MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, t)
    MOCK_METHOD_END(time_t, time(t));

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, psz)
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE)

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, handle, const char*, s2)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, TEST_CHAR)

    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE);

    MOCK_STATIC_METHOD_1(, STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle)
    MOCK_METHOD_END(STRING_TOKENIZER_HANDLE, TEST_STRING_TOKENIZER_HANDLE);

    MOCK_STATIC_METHOD_3(, int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters)
    MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_1(, void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    /* Version Mocks */
    MOCK_STATIC_METHOD_0(, const char*, IoTHubClient_GetVersionString)
    MOCK_METHOD_END(const char*, (const char*) NULL)

    MOCK_STATIC_METHOD_0(, TICK_COUNTER_HANDLE, tickcounter_create);
        TICK_COUNTER_HANDLE result2 = (TICK_COUNTER_HANDLE )BASEIMPLEMENTATION::gballoc_malloc(1);
    MOCK_METHOD_END(TICK_COUNTER_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter);
        BASEIMPLEMENTATION::gballoc_free(tick_counter);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms);
    MOCK_METHOD_END(int, 0)

		// vector.h
	MOCK_STATIC_METHOD_1(, VECTOR_HANDLE, VECTOR_create, size_t, elementSize)
		VECTOR_HANDLE result2;
		result2 = BASEIMPLEMENTATION::VECTOR_create(elementSize);
	MOCK_METHOD_END(VECTOR_HANDLE, result2)

	MOCK_STATIC_METHOD_1(, void, VECTOR_destroy, VECTOR_HANDLE, vector)
		BASEIMPLEMENTATION::VECTOR_destroy(vector);
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_3(, int, VECTOR_push_back, VECTOR_HANDLE, vector, const void*, elements, size_t, numElements)
		int result2;
		result2 = BASEIMPLEMENTATION::VECTOR_push_back(vector, elements, numElements);
	MOCK_METHOD_END(int, result2)

	MOCK_STATIC_METHOD_3(, void, VECTOR_erase, VECTOR_HANDLE, vector, void*, elements, size_t, numElements)
		BASEIMPLEMENTATION::VECTOR_erase(vector, elements, numElements);
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_2(, void*, VECTOR_element, VECTOR_HANDLE, vector, size_t, index)
		void* result2 = BASEIMPLEMENTATION::VECTOR_element(vector, index);
	MOCK_METHOD_END(void*, result2)

	MOCK_STATIC_METHOD_1(, void*, VECTOR_front, VECTOR_HANDLE, vector)
		void* result2 = BASEIMPLEMENTATION::VECTOR_front(vector);
	MOCK_METHOD_END(void*, result2)

	MOCK_STATIC_METHOD_1(, void*, VECTOR_back, const VECTOR_HANDLE, vector)
		void* result2 = BASEIMPLEMENTATION::VECTOR_back(vector);
	MOCK_METHOD_END(void*, result2)

	MOCK_STATIC_METHOD_3(, void*, VECTOR_find_if, VECTOR_HANDLE, vector, PREDICATE_FUNCTION, pred, const void*, value)
		void* result2;
		result2 = BASEIMPLEMENTATION::VECTOR_find_if(vector, pred, value);
	MOCK_METHOD_END(void*, result2)

	MOCK_STATIC_METHOD_1(, size_t, VECTOR_size, VECTOR_HANDLE, vector)
		size_t result2 = BASEIMPLEMENTATION::VECTOR_size(vector);
	MOCK_METHOD_END(size_t, result2)


	/* ThreadAPI mocks */
	MOCK_STATIC_METHOD_3(, THREADAPI_RESULT, ThreadAPI_Create, THREAD_HANDLE*, threadHandle, THREAD_START_FUNC, func, void*, arg);
		*threadHandle = TEST_THREAD_HANDLE;
		threadFunc = func;
		threadFuncArg = arg;
	MOCK_METHOD_END(THREADAPI_RESULT, THREADAPI_OK);
	MOCK_STATIC_METHOD_2(, THREADAPI_RESULT, ThreadAPI_Join, THREAD_HANDLE, threadHandle, int*, res);
	MOCK_METHOD_END(THREADAPI_RESULT, THREADAPI_OK);
	MOCK_STATIC_METHOD_1(, void, ThreadAPI_Exit, int, res);
	MOCK_VOID_METHOD_END();
	MOCK_STATIC_METHOD_1(, void, ThreadAPI_Sleep, unsigned int, milliseconds)
		//if ((howManyDoWorkCalls > 0) && (howManyDoWorkCalls == doWorkCallCount))
		//{
		//}
	MOCK_VOID_METHOD_END();

		/* Lock mocks */
	MOCK_STATIC_METHOD_0(, LOCK_HANDLE, Lock_Init);
	MOCK_METHOD_END(LOCK_HANDLE, TEST_LOCK_HANDLE);
	MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock, LOCK_HANDLE, handle);
	MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);
	MOCK_STATIC_METHOD_1(, LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
	MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);
	MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);
	MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);

};

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, gballoc_free, void*, ptr)

DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_SetOption, TRANSPORT_LL_HANDLE, handle, const char*, optionName, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , TRANSPORT_LL_HANDLE, FAKE_IoTHubTransport_Create,const IOTHUBTRANSPORT_CONFIG*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_Destroy, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_5(CIotHubTransportMocks, , IOTHUB_DEVICE_HANDLE, FAKE_IoTHubTransport_Register, TRANSPORT_LL_HANDLE, handle, const char*, deviceId, const char*, deviceKey, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, PDLIST_ENTRY, waitingToSend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_Unregister, IOTHUB_DEVICE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , int, FAKE_IoTHubTransport_Subscribe, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_Unsubscribe, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_DoWork, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);

DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, messageCallback, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback);


DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , time_t, get_time, time_t*, t);

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , STRING_HANDLE, STRING_construct, const char*, psz);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , int, STRING_concat, STRING_HANDLE, handle, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, , const char*, IoTHubClient_GetVersionString);

DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, ,TICK_COUNTER_HANDLE, tickcounter_create);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms);

//vector
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , VECTOR_HANDLE, VECTOR_create, size_t, elementSize);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, VECTOR_destroy, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , int, VECTOR_push_back, VECTOR_HANDLE, vector, const void*, elements, size_t, numElements);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , void, VECTOR_erase, VECTOR_HANDLE, vector, void*, elements, size_t, numElements);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void*, VECTOR_element, VECTOR_HANDLE, vector, size_t, index);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void*, VECTOR_front, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void*, VECTOR_back, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , void*, VECTOR_find_if, VECTOR_HANDLE, vector, PREDICATE_FUNCTION, pred, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , size_t, VECTOR_size, VECTOR_HANDLE, vector);

DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , THREADAPI_RESULT, ThreadAPI_Create, THREAD_HANDLE*, threadHandle, THREAD_START_FUNC, func, void*, arg);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , THREADAPI_RESULT, ThreadAPI_Join, THREAD_HANDLE, threadHandle, int*, res);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, ThreadAPI_Exit, int, res);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, ThreadAPI_Sleep, unsigned int, milliseconds);


DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, , LOCK_HANDLE, Lock_Init);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , LOCK_RESULT, Lock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);

static TRANSPORT_PROVIDER FAKE_transport_provider =
{
    FAKE_IoTHubTransport_SetOption,     /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;       */
    FAKE_IoTHubTransport_Create,        /*pfIoTHubTransport_Create IoTHubTransport_Create;              */
    FAKE_IoTHubTransport_Destroy,       /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;            */
	FAKE_IoTHubTransport_Register,		/* pfIotHubTransport_Register IoTHubTransport_Register;         */
	FAKE_IoTHubTransport_Unregister,    /* pfIotHubTransport_Unregister IoTHubTransport_Unegister;      */
    FAKE_IoTHubTransport_Subscribe,     /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;        */
    FAKE_IoTHubTransport_Unsubscribe,   /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;    */
    FAKE_IoTHubTransport_DoWork,        /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;              */
    FAKE_IoTHubTransport_GetSendStatus  /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus; */
};

static const void* provideFAKE(void)
{
    return &FAKE_transport_provider; /*by convention... */
}

BEGIN_TEST_SUITE(iothubtransport_unittests)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        
        INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        MicroMockDestroyMutex(g_testByTest);
        DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (!MicroMockAcquireMutex(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }
        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;
		checkProtocolGatewayHostName = false;
		checkProtocolGatewayIsNull = false;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

	TEST_FUNCTION(IoTHubTransport_Create_success_returns_non_null)
	{
		CIotHubTransportMocks mocks;
		///arrange
		STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(mocks, Lock_Init());
		STRICT_EXPECTED_CALL(mocks, VECTOR_create(sizeof(IOTHUB_CLIENT_HANDLE)));

		///act
		auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);

		///assert

		ASSERT_IS_NOT_NULL(result);

		///ablution
	}
END_TEST_SUITE(iothubtransport_unittests)

