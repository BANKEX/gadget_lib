#include "annealing.hpp"
#include "basic_gadgets.hpp"

using namespace gadgetlib;

void check_sha256()
{
	gadget input(32, false, 0x33323138);
	gadget result(32, true, 0x9D21310B);
	gadget comparison = ((sha256_gadget(input))[{uint32_t(224), uint32_t(255)}] == result);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_sha256_part()
{
	gadget input(24, false, 0x616263);


	auto bitsize = input.get_bitsize();

	gadget h0 = { 0x6A09E667, (uint32_t)32 }, h1 = { 0xBB67AE85, (uint32_t)32 },
		h2 = { 0x3C6EF372, (uint32_t)32 }, h3 = { 0xA54FF53A, (uint32_t)32 },
		h4 = { 0x510E527F, (uint32_t)32 }, h5 = { 0x9B05688C, (uint32_t)32 },
		h6 = { 0x1F83D9AB, (uint32_t)32 }, h7 = { 0x5BE0CD19, (uint32_t)32 };

	uint32_t k_arr[] = {
		0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1,
		0x923F82A4, 0xAB1C5ED5, 0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
		0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174, 0xE49B69C1, 0xEFBE4786,
		0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
		0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147,
		0x06CA6351, 0x14292967, 0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
		0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85, 0xA2BFE8A1, 0xA81A664B,
		0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
		0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A,
		0x5B9CCA4F, 0x682E6FF3, 0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
		0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
	};

	//Preliminary processing :
	uint32_t temp_len = (input.get_bitsize() + 1) % 512;
	uint32_t padding_len = ((temp_len <= 448) ? 448 - temp_len :
		448 + (512 - temp_len));
	gadget padded_message = input || gadget(1, (uint32_t)1);
	while (padding_len > 0)
	{
		if (padding_len >= 32)
		{
			padded_message = padded_message || gadget(0, (uint32_t)32);
			padding_len -= 32;
		}
		else
		{
			padded_message = padded_message || gadget(0, (uint32_t)padding_len);
			padding_len = 0;
		}
	}

	padded_message = padded_message || gadget(0, (uint32_t)32) || gadget(bitsize, (uint32_t)32);


	uint32_t final_len = padded_message.get_bitsize();
	gadget w[64];
	for (unsigned index = 0; index < final_len / 512; index++)
	{
		
		//for (auto i = 0; i < 16; i++)
			//w[i] = padded_message[{512 * index + i * 32, 512 * index + i * 32 + 31}];

		for (auto i = 16; i <= 63; i++)
		{
			/*gadget s0 = w[i - 15].rotate_right(7) ^ w[i - 15].rotate_right(18) ^
				(w[i - 15] >> 3);
			gadget s1 = w[i - 2].rotate_right(17) ^ w[i - 2].rotate_right(19) ^
				(w[i - 2] >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;*/
		}
	}

	gadget result(32, true, 0x18);
	gadget comparison = (padded_message[{(uint32_t)(15 * 32), 15 * 32 + 31}] == result);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_addition()
{
	gadget input(32, false, 0x12345678);
	gadget result(32, true, 0xc1840208);
	gadget const_gadget1(0xf0e21561, (uint32_t)32);
	gadget const_gadget2(0xdeadbeef, (uint32_t)32);
	gadget comparison = (((input ^ const_gadget1) + const_gadget2) == result);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_concat_extract()
{
	gadget input = gadget(0, (uint32_t)32) || gadget(0x1, (uint32_t)1) || gadget(0x0, (uint32_t)31) ||
		gadget(0x2, (uint32_t)32) || gadget(3, (uint32_t)32) || gadget(4, (uint32_t)32) ||
		gadget(5, (uint32_t)32) || gadget(6, (uint32_t)32) || gadget(7, (uint32_t)32);
	gadget result(32, true, 0x7);
	gadget comparison = ((input)[{(uint32_t)(32 * 7), (uint32_t)(32 * 7 +31)}] == result);


	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_concat_extract2()
{
	gadget input = gadget(1, (uint32_t)16) || gadget(2, (uint32_t)16);
	gadget result = { 0x00010002, (uint32_t)32 };
	gadget comparison = (input == result);


	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	example.dump();
	getchar();
}

void check_shr()
{
	gadget input(32, false, 0x12345678);
	gadget result(32, true, 0x01234567);
	gadget const_gadget(0x21436587, (uint32_t)32);
	gadget comparison = ((input >> 4)  == result);


	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_rotate()
{
	gadget input(32, false, 0x12345678);
	gadget result(32, true, 0x81234567);
	gadget const_gadget(0x21436587, (uint32_t)32);
	gadget comparison = ((input.rotate_right(32)) == input);


	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	example.dump();
	getchar();
}

void check_and()
{
	gadget input(32, false, 0x1122335E);
	gadget result(32, true, 0x1020324e);
	gadget const_gadget(0xdeadbeef, (uint32_t)32);
	gadget comparison = ((input & const_gadget) == result);


	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_not()
{
	gadget input(32, false, 0xffffffff);
	gadget result(32, true, 0x00000000);
	gadget comparison = ((!input) == result);


	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_merkle_proof()
{
	std::vector<uint32_t> leaves = { 1, 2, 3, 4};
	MerkleTree tree(leaves);
	uint32_t raw_address = 0;
	gadget address(tree.height, true, raw_address);
	//gadget leaf(32, false, tree.get_leaf_at_address(raw_address));
	gadget leaf(32, false, 0x1);
	std::vector<gadget> proof = tree.get_proof(raw_address);
	gadget merkle_root = tree.get_root();
	gadget flag = (merkle_tree_proof(address, leaf, proof, tree.height) == merkle_root);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, flag);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;

	getchar();
}

void check_temp_merkle_proof()
{
	std::vector<uint32_t> leaves = { 1, 2 };
	MerkleTree tree(leaves);
	uint32_t raw_address = 0;
	gadget address(tree.height, true, raw_address);
	gadget leaf(32, false, tree.get_leaf_at_address(raw_address));
	std::vector<gadget> proof = { gadget(2, (uint32_t)32) };
	gadget merkle_root = { gadget(0x00010002, (uint32_t)32) };
	gadget flag = (test_merkle_tree(address, leaf, proof, tree.height) == merkle_root);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, flag);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_ITE()
{
	gadget first_var(32, false, 0xdeadbeef);
	gadget second_var(32, false, 0x12345678);
	gadget result(32, true, 0x12345678);
	gadget input(1, true, 0);

	gadget comparison = (ITE(input, first_var, second_var) == result);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

#include "sha256.hpp"

std::string hexlify2(std::string hash)
{
	auto convert2ch = [](char c) -> char
	{
		if (c >= '0' && c <= '9')
			return (c - '0');
		if (c >= 'a' && c <= 'f')
			return (c - 'a' + 10);
	};

	std::string result(hash.size() / 2, 'x');
	assert(hash.size() % 2 == 0);
	auto j = 0;
	for (size_t i = 0; i < hash.size() / 2; i++)
	{
		result[i] = convert2ch(hash[j++]) * 16;
		result[i] += convert2ch(hash[j++]);
	}
	return result;
}

void check_pico_sha256()
{
	std::string input_str = "3218";
	std::string hex_digest;
	picosha2::hash256_hex_string(input_str, hex_digest);
	std::string result_str = hexlify2(hex_digest);
	
	std::vector<unsigned char> result_vec;
	result_vec.assign(result_str.data(), result_str.data() + result_str.size());

	gadget input(32, true, 0x33323138);
	gadget result(result_vec, 256, true);
	gadget comparison = (stub_hash(input) == result);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, comparison);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
	getchar();
}

void check_transaction()
{
	std::vector<uint32_t> leaves = { 1, 2};
	MerkleTree tree(leaves);
	uint32_t raw_from_address = 0;
	uint32_t raw_to_address = 1;

	gadget check = check_transaction(raw_from_address, raw_to_address, 1, tree);
	gadget success = (1, uint32_t(1));
	gadget flag = (check == success);

	auto pboard = protoboard<int64_t>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, flag);
	r1cs_example<int64_t> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;

	getchar();
}

int main(int argc, char* argv[])
{
	check_transaction();
}