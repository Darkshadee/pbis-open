/*
 * Copyright © BeyondTrust Software 2004 - 2019
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * BEYONDTRUST MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING TERMS AS
 * WELL. IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT WITH
 * BEYONDTRUST, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE TERMS OF THAT
 * SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE APACHE LICENSE,
 * NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU HAVE QUESTIONS, OR WISH TO REQUEST
 * A COPY OF THE ALTERNATE LICENSING TERMS OFFERED BY BEYONDTRUST, PLEASE CONTACT
 * BEYONDTRUST AT beyondtrust.com/contact
 */

/*
 * Module Name:
 *
 *        marshal-private.h
 *
 * Abstract:
 *
 *        Marshalling API (private header)
 *
 * Authors: Brian Koropoff (bkoropoff@likewisesoftware.com)
 *
 */
#ifndef __LWMSG_MARSHAL_PRIVATE_H__
#define __LWMSG_MARSHAL_PRIVATE_H__

#include <lwmsg/data.h>
#include "type-private.h"
#include "status-private.h"
#include "util-private.h"

typedef uint32_t LWMsgObjectID;

typedef struct LWMsgObjectMapEntry
{
    LWMsgRing ring1;
    LWMsgRing ring2;
    LWMsgObjectID id;
    void* object;
    LWMsgTypeSpec* spec;
} LWMsgObjectMapEntry;

typedef struct LWMsgObjectMap
{
    LWMsgObjectID next_id;
    LWMsgHashTable hash_by_object;
    LWMsgHashTable hash_by_id;
} LWMsgObjectMap;

struct LWMsgDataContext
{
    const LWMsgContext* context;
    LWMsgByteOrder byte_order;
};

typedef struct LWMsgMarshalState
{
    unsigned char* dominating_object;
    LWMsgObjectMap* map;
} LWMsgMarshalState;

#define MAX_INTEGER_SIZE (16)

#define MARSHAL_RAISE_ERROR(hand, expr, ...) BAIL_ON_ERROR(status = RAISE((hand)->context, (expr), __VA_ARGS__))

typedef struct LWMsgUnmarshalState
{
    unsigned char* dominating_object;
    LWMsgObjectMap* map;
} LWMsgUnmarshalState;

typedef LWMsgStatus (*LWMsgGraphVisitFunction) (
    LWMsgTypeIter* iter,
    unsigned char* object,
    void* data
    );

LWMsgStatus
lwmsg_data_object_map_find_id(
    LWMsgObjectMap* map,
    LWMsgObjectID id,
    LWMsgTypeIter* iter,
    void** object
    );

LWMsgStatus
lwmsg_data_object_map_find_object(
    LWMsgObjectMap* map,
    void* object,
    LWMsgObjectID* id
    );

LWMsgStatus
lwmsg_data_object_map_insert(
    LWMsgObjectMap* map,
    void* object,
    LWMsgTypeIter* iter,
    LWMsgObjectID* id
    );

void
lwmsg_data_object_map_destroy(
    LWMsgObjectMap* map
    );

LWMsgStatus
lwmsg_data_visit_graph(
    LWMsgTypeIter* iter,
    unsigned char* object,
    LWMsgGraphVisitFunction func,
    void* data
    );

LWMsgStatus
lwmsg_data_visit_graph_children(
    LWMsgTypeIter* iter,
    unsigned char* object,
    LWMsgGraphVisitFunction func,
    void* data
    );

LWMsgStatus
lwmsg_data_extract_discrim_tag(
    LWMsgTypeIter* iter,
    unsigned char* dominating_struct,
    uint32_t* tag
    );

LWMsgStatus
lwmsg_data_extract_length(
    LWMsgTypeIter* iter,
    unsigned char* dominating_struct,
    size_t *length
    );

LWMsgStatus
lwmsg_data_extract_active_arm(
    LWMsgDataContext* context,
    LWMsgTypeIter* iter,
    unsigned char* dominating_struct,
    LWMsgTypeIter* active_iter
    );

LWMsgStatus
lwmsg_data_decode_enum_value(
    LWMsgDataContext* context,
    LWMsgTypeIter* iter,
    uint64_t value,
    uint64_t* mask,
    uint64_t* res
    );

LWMsgStatus
lwmsg_data_verify_range(
    LWMsgDataContext* context,
    LWMsgTypeIter* iter,
    void* object,
    size_t object_size
    );

LWMsgStatus
lwmsg_data_calculate_indirect_metrics(
    LWMsgTypeIter* iter,
    unsigned char* object,
    size_t* count,
    size_t* element_size
    );

LWMsgStatus
lwmsg_data_free_graph_internal(
    LWMsgDataContext* context,
    LWMsgTypeIter* iter,
    unsigned char* object
    );

/**
 * @brief Print textual representation of a data graph
 *
 * Prints a human-readable textual representation of a data graph.
 * The printed form will be written into the provided #LWMsgBuffer.
 * Consider using #lwmsg_data_print_graph_alloc() if you want to easily
 * print to a string.
 *
 * @param[in,out] context the data context
 * @param[in] type a type specification which describes the graph root
 * @param[in] root the root of the data graph
 * @param[in] indent the level of indentation at which to print
 * @param[in,out] buffer the buffer into which to print
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_code{MALFORMED, the provided data did not conform in some way to the provided type information}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_data_print_graph(
    LWMsgDataContext* context,
    LWMsgTypeSpec* type,
    void* root,
    unsigned int indent,
    LWMsgBuffer* buffer
    );

LWMsgStatus
lwmsg_data_raise(
    LWMsgDataContext* context,
    LWMsgTypeIter* iter,
    LWMsgStatus status,
    const char* function,
    const char* filename,
    unsigned int line,
    const char* format,
    ...
    );

#define DATA_RAISE(ctx, iter, status, ...) \
    (lwmsg_data_raise((ctx), (iter), (status), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))

#endif
