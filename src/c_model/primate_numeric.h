#include <iostream>
#include <string>

using namespace std;

class primate_hexuint {
public:
	primate_hexuint(int size) : size(size*2) {}
	primate_hexuint(const string &val, int len) {
		size = len*2;
		if (val.length() > size) {
			data = val.substr(val.length() - size, size);
		} else if (val.length() < size) {
			data = val;
			for (int i = 0; i < size - val.length(); i++) data = "0" + data;
		} else {
			data = val;
		}
	}

	void set(const string &val) {
		if (val.length() > size) {
			data = val.substr(val.length() - size, size);
		} else if (val.length() < size) {
			data = val;
			for (int i = 0; i < size - val.length(); i++) data = "0" + data;
		} else {
			data = val;
		}
	}

	void copy(const primate_hexuint &val) {
		data = val.data;
		size = val.size;
	}

	int get_size() { return size/2; }

	void cat(const primate_hexuint &val) {
		data = val.data + data;
		size += val.size;
	}

	inline primate_hexuint operator () (const int end_idx, const int start_idx) {
		int new_len = end_idx - start_idx + 1;
		string tmp = data.substr((size - (end_idx+1)*2), new_len*2);
		return primate_hexuint(tmp, new_len);
	}

	inline primate_hexuint operator >> (const int shfr) {
		string tmp;
		if (shfr*2 < data.length()) {
			tmp = data.substr(0, data.length() - shfr*2);
		} else {
			tmp = "0";
		}
		return primate_hexuint(tmp, size);
	}

	inline void shiftRT(const int shfr) {
		if (shfr*2 < data.length()) {
			data = data.substr(0, data.length() - shfr*2);
			size -= (shfr*2);
		} else {
			data = "";
			size = 0;
		}
	}

	inline bool operator== (const primate_hexuint &val) const {
		return (data == val.data);
	}

	inline bool operator== (const string &val) const {
		if (val.length() < size) {
			string tmp = val;
			for (int i = 0; i < size - val.length(); i++)
				tmp = "0" + tmp;
			return (data == tmp);
		} else if (val.length() > size) {
			string tmp = data;
			for (int i = 0; i < val.length() - size; i++)
				tmp = "0" + tmp;
			return (tmp == val);
		} else {
			return (data == val);
		}
	}

	inline bool operator!= (const primate_hexuint &val) const {
		return (data != val.data);
	}

	inline primate_hexuint& operator=(const primate_hexuint &val) {
		if (val.data.length() > size) {
			data = val.data.substr(val.data.length() - size, size);
		} else {
			data = val.data;
		}
		return (*this);
	}

public:
	int size;
	string data;
};

inline ostream& operator<<(ostream &os, const primate_hexuint& val) {
	os << val.data;
	return os;
}