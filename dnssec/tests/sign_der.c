#include <tap/basic.h>
#include <stdint.h>
#include <string.h>

#include "binary.h"
#include "error.h"
#include "sign/der.h"

/*!
 * Check conversion of Dss-Sig-Value to parameters.
 */
void raw_to_params(void)
{
	const dnssec_binary_t raw = { .size = 103, .data = (uint8_t []) {
		0x30,0x65,0x02,0x31,0x00,0xcd,0xe6,0xf6,0xf5,0x2e,
		0x4b,0xd9,0xfe,0x32,0xf7,0x94,0x3d,0x60,0x5a,0x5f,
		0x17,0x0f,0x2e,0xec,0xd2,0xc2,0x6d,0x79,0xba,0x85,
		0xf0,0x57,0xf4,0x15,0x2d,0x04,0xbd,0xb8,0xc9,0x24,
		0x15,0x9a,0x9b,0x74,0x91,0x75,0xb0,0x22,0x47,0x85,
		0x19,0x45,0x58,0x02,0x30,0x5b,0xe2,0x9d,0x18,0x71,
		0x5d,0xb3,0xd8,0x0b,0x5d,0xee,0x9c,0x38,0x79,0x46,
		0x29,0x90,0x89,0xd5,0x21,0xb0,0x82,0x45,0x78,0x2e,
		0xf0,0x68,0xa9,0xfa,0x76,0xaa,0xe2,0x36,0x47,0xcc,
		0x3c,0x74,0x2d,0xfc,0x6d,0xa6,0x72,0x35,0x29,0xc7,
		0x21,0xcf,0x5c
	}};

	const dnssec_binary_t expected_r = { .size = 49, .data = (uint8_t []) {
		0x00,0xcd,0xe6,0xf6,0xf5,0x2e,0x4b,0xd9,0xfe,0x32,
		0xf7,0x94,0x3d,0x60,0x5a,0x5f,0x17,0x0f,0x2e,0xec,
		0xd2,0xc2,0x6d,0x79,0xba,0x85,0xf0,0x57,0xf4,0x15,
		0x2d,0x04,0xbd,0xb8,0xc9,0x24,0x15,0x9a,0x9b,0x74,
		0x91,0x75,0xb0,0x22,0x47,0x85,0x19,0x45,0x58
	}};

	const dnssec_binary_t expected_s = { .size = 48, .data = (uint8_t []) {
		0x5b,0xe2,0x9d,0x18,0x71,0x5d,0xb3,0xd8,0x0b,0x5d,
		0xee,0x9c,0x38,0x79,0x46,0x29,0x90,0x89,0xd5,0x21,
		0xb0,0x82,0x45,0x78,0x2e,0xf0,0x68,0xa9,0xfa,0x76,
		0xaa,0xe2,0x36,0x47,0xcc,0x3c,0x74,0x2d,0xfc,0x6d,
		0xa6,0x72,0x35,0x29,0xc7,0x21,0xcf,0x5c
	}};

	dnssec_binary_t value_r = { 0 };
	dnssec_binary_t value_s = { 0 };

	int result = dss_sig_value_decode(&raw, &value_r, &value_s);

	ok(result == DNSSEC_EOK, "dss_sig_value_decode() exit code");
	ok(value_r.size == expected_r.size && value_r.data != NULL &&
	   memcmp(value_r.data, expected_r.data, expected_r.size) == 0,
	   "dss_sig_value_decode() correct r value");
	ok(value_s.size == expected_s.size && value_s.data != NULL &&
	   memcmp(value_s.data, expected_s.data, expected_s.size) == 0,
	   "dss_sig_value_decode() correct s value");
}

/*!
 * Check conversion from parameters to Dss-Sig-Value.
 */
void params_to_raw(void)
{
	const dnssec_binary_t value_r = { .size = 32, .data = (uint8_t []) {
		0x2e,0x79,0xdd,0x3e,0xec,0x80,0x8c,0xd7,0x0c,0x22,
		0x51,0x28,0xb3,0xcd,0x28,0x8e,0xc6,0x24,0xe1,0xfc,
		0xd7,0x37,0xdb,0xc1,0x17,0xbb,0x88,0x63,0xec,0x1a,
		0x3c,0x7c
	}};
	const dnssec_binary_t value_s = { .size = 33, .data = (uint8_t []) {
		0x00,0x8c,0x40,0x58,0x99,0x53,0x43,0x5d,0xcb,0x21,
		0xf0,0x81,0x6d,0xd3,0x02,0x67,0x85,0x8f,0x4a,0x34,
		0x9c,0x2a,0x21,0x47,0xf3,0x76,0x48,0xdf,0xfe,0x52,
		0x54,0xf9,0xa5,
	}};

	const dnssec_binary_t expected_raw = { .size = 71, .data = (uint8_t []) {
		0x30,0x45,0x02,0x20,0x2e,0x79,0xdd,0x3e,0xec,0x80,
		0x8c,0xd7,0x0c,0x22,0x51,0x28,0xb3,0xcd,0x28,0x8e,
		0xc6,0x24,0xe1,0xfc,0xd7,0x37,0xdb,0xc1,0x17,0xbb,
		0x88,0x63,0xec,0x1a,0x3c,0x7c,0x02,0x21,0x00,0x8c,
		0x40,0x58,0x99,0x53,0x43,0x5d,0xcb,0x21,0xf0,0x81,
		0x6d,0xd3,0x02,0x67,0x85,0x8f,0x4a,0x34,0x9c,0x2a,
		0x21,0x47,0xf3,0x76,0x48,0xdf,0xfe,0x52,0x54,0xf9,
		0xa5,
	}};

	dnssec_binary_t raw = { 0 };

	int result = dss_sig_value_encode(&raw, &value_r, &value_s);

	ok(result == DNSSEC_EOK, "dss_sig_value_encode() exit code");
	ok(raw.size == expected_raw.size && raw.data != NULL &&
	   memcmp(raw.data, expected_raw.data, expected_raw.size) == 0,
	   "dss_sig_value_encode() correct der value");

	dnssec_binary_free(&raw);
}

int main(void)
{
	plan_lazy();

	raw_to_params();
	params_to_raw();

	return 0;
}