#include "primate_p4.h"

void p4_parse_sw(primate_io &top_intf) {
	// Parse
	ethernet_t eth;
	ptp_l_t ptp_l;
	ptp_h_t ptp_h;
	header_t header_0;
	header_t header_1;
	standard_metadata_t standard_metadata;
	standard_metadata.egress_spec.set("0000");
	standard_metadata.mcast_grp.set("0000");
	uint16_t hdr_valid = 0;
	top_intf.Input_header<ethernet_t>(14, eth);
	if (eth.etherType == primate_hexuint("88f7", 2)) {
		top_intf.Input_header<ptp_l_t>(20, ptp_l);
		top_intf.Input_header<ptp_h_t>(24, ptp_h);
		hdr_valid++;
		if (ptp_l.reserved2 == primate_hexuint("01", 1)) {
			top_intf.Input_header<header_t>(8, header_0);
			hdr_valid++;
			if (header_0.field_0 != primate_hexuint("0000", 2)) {
				top_intf.Input_header<header_t>(8, header_1);
				hdr_valid++;
			}
		}
	}
	top_intf.Input_done();
	cout << eth.dstAddr << " " << eth.srcAddr << " " << eth.etherType << endl;
	cout << hdr_valid << endl;
	// Ingress
	int flag;
	primate_hexuint port(2);
	flag = forward_exact(eth.dstAddr, port);
	if (flag == 0) {
		//ipv4 forward
		standard_metadata.egress_spec = port;
	} else {
		//default drop
		standard_metadata.egress_spec.set("01ff");
	}
	top_intf.Output_meta(standard_metadata);
	top_intf.Output_header<ethernet_t>(14, eth);
	if (hdr_valid > 0) {
		top_intf.Output_header<ptp_l_t>(20, ptp_l);
		top_intf.Output_header<ptp_h_t>(24, ptp_h);
		if (hdr_valid > 1) {
			top_intf.Output_header<header_t>(8, header_0);
			if (hdr_valid > 2) {
				top_intf.Output_header<header_t>(8, header_1);
			}
		}
	}
	top_intf.Output_done();
}

int main() {
	primate_io top_intf("input.txt", "output.txt");
	p4_parse_sw(top_intf);
	return 0;
}
