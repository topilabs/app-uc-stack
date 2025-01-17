/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.7 */

#ifndef PB_MESSAGE_PB_H_INCLUDED
#define PB_MESSAGE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _DataPackage {
    uint32_t potentiometer;
    uint32_t generator;
} DataPackage;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define DataPackage_init_default                 {0, 0}
#define DataPackage_init_zero                    {0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define DataPackage_potentiometer_tag            1
#define DataPackage_generator_tag                2

/* Struct field encoding specification for nanopb */
#define DataPackage_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   potentiometer,     1) \
X(a, STATIC,   SINGULAR, UINT32,   generator,         2)
#define DataPackage_CALLBACK NULL
#define DataPackage_DEFAULT NULL

extern const pb_msgdesc_t DataPackage_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define DataPackage_fields &DataPackage_msg

/* Maximum encoded size of messages (where known) */
#define DataPackage_size                         12

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
