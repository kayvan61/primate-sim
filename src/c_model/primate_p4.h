#include <iostream>
#include <fstream>
#include "p4_parse_simple.h"

using namespace std;

class primate_io {
public:
	primate_io(string filename_in, string filename_out) {
		infile.open(filename_in);
		outfile.open(filename_out);
	}

	template<typename h_t>
	void Input_header(const int &length, h_t &header) {
		if (buf_data.get_size() < length) {
			infile >> last >> empty >> indata;
			buf_data.cat(primate_hexuint(indata, 64));
		}
		header.set(buf_data);
		buf_data.shiftRT(length);
	} // inputExtracti inst

	void Input_done() {
		if (last) {
			pkt_buf.copy(buf_data);
		} else {
			pkt_buf.copy(buf_data);
			do {
				infile >> last >> empty >> indata;
				pkt_buf.cat(primate_hexuint(indata, 64));
			} while (!last);
		}
	} // inputDone inst

	template<typename h_t>
	void Output_header(const int &length, h_t &header) {
		out_buf.cat(header.marshal());
	}

	void Output_done() {
		out_buf.cat(pkt_buf);
		int length = 0;
		while (length + 64 < out_buf.get_size()) {
			outfile << "0 0 " << out_buf(length + 63, length).data << endl;
			length += 64;
		}
		outfile << "1 " << 64 - (out_buf.get_size() - length) << " " << out_buf(out_buf.get_size() - 1, length) << endl;
	}
	void Output_meta(standard_metadata_t &standard_metadata) {
		cout << "port: " << standard_metadata.egress_spec << endl;
	} //outputMeta inst

private:
	ifstream infile;
	ofstream outfile;

	string indata;
	int empty;
	bool last;

	primate_hexuint buf_data{0};
	primate_hexuint out_buf{0};
	primate_hexuint pkt_buf{0};
};

// #pragma primate blue matchTable 1 1
int forward_exact(primate_hexuint &dstAddr, primate_hexuint &port) {
	if (dstAddr == "0cc47aa32534") {
		port.set("0001");
		return 0;
	} else if (dstAddr == "0cc47aa32535") {
		port.set("0002");
		return 0;
	}
	return 1;
}