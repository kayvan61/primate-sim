#include <stdint.h>
#include "primate_numeric.h"

typedef struct
{
    primate_hexuint dstAddr{6};
    primate_hexuint srcAddr{6};
    primate_hexuint etherType{2};

    void set(primate_hexuint &buf_data) {
        dstAddr = buf_data(5, 0);
        srcAddr = buf_data(11, 6);
        etherType = buf_data(13, 12);
    }

    primate_hexuint marshal() {
        string tmp = etherType.data + srcAddr.data + dstAddr.data;
        return primate_hexuint(tmp, 14);
    }
} ethernet_t;

typedef struct
{
    primate_hexuint transportSpecific_domainNumber{5};
    primate_hexuint reserved2{1};
    primate_hexuint flags_reserved3{14};

    void set(primate_hexuint &buf_data) {
        transportSpecific_domainNumber = buf_data(4, 0);
        reserved2 = buf_data(5, 5);
        flags_reserved3 = buf_data(19, 6);
    }

    primate_hexuint marshal() {
        string tmp = flags_reserved3.data + reserved2.data + transportSpecific_domainNumber.data;
        return primate_hexuint(tmp, 20);
    }
} ptp_l_t;

typedef struct
{
    primate_hexuint sourcePortIdentity_originTimestamp{24};
    void set(primate_hexuint &buf_data) {
        sourcePortIdentity_originTimestamp = buf_data(23, 0);
    }

    primate_hexuint marshal() {
        return sourcePortIdentity_originTimestamp;
    }
} ptp_h_t;

typedef struct
{
    primate_hexuint field_0{2};
    primate_hexuint field_1_field_3{6};
    void set(primate_hexuint &buf_data) {
        field_0 = buf_data(1, 0);
        field_1_field_3 = buf_data(7, 2);
    }

    primate_hexuint marshal() {
        string tmp = field_1_field_3.data + field_0.data;
        return primate_hexuint(tmp, 8);
    }
} header_t;

typedef struct
{
    primate_hexuint egress_spec{2};
    primate_hexuint mcast_grp{2};

    primate_hexuint marshal() {
        string tmp = mcast_grp.data + egress_spec.data;
        return primate_hexuint(tmp, 8);
    }
} standard_metadata_t;