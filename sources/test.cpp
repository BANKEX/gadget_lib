#include "annealing.hpp"
#include "basic_gadgets.hpp"
#include "utils.hpp"
#include "Field.hpp"


using namespace gadgetlib;

#define PRIME_NUM 1273437928060067303
//#define PRIME_NUM 11

struct my_str_provider
{
	constexpr static char const* str()
	{ 
		return "245521870800009915270890634279110554381234874750592897213830758155042977723351";
	}
};

void check(const gadget& gadget)
{
	auto pboard = protoboard<Field<PRIME_NUM>>();
	auto annealing = engraver();
	annealing.incorporate_gadget(pboard, gadget);
	r1cs_example<Field<PRIME_NUM>> example(pboard);
	std::cout << example.constraint_system.size() << std::endl;
	std::cout << example.check_assignment() << std::endl;
	//example.dump();
}

void check_addition()
{
	gadget input(0x12345678, 32, false);
	gadget result(0xf0e21567, 32, true);
	gadget const_gadget(0xdeadbeef, 32);
	gadget comparison = ((input  + const_gadget) == result);

	check(comparison);
}


void check_addition_xor()
{
	gadget input(0x12345678, 32, false);
	gadget result(0xc1840208, 32, true);
	gadget const_gadget1(0xf0e21561, 32);
	gadget const_gadget2(0xdeadbeef, 32);
	gadget comparison = (((input ^ const_gadget1) + const_gadget2) == result);

	check(comparison);
}

void check_concat_extract()
{
	gadget input = gadget(0, 32) || gadget(0x1, 1) || gadget(0x0, 31) || gadget(0x2, 32) 
		|| gadget(3, 32) || gadget(4, 32) || gadget(5, 32) || gadget(6, 32) || 
		gadget(7, 32);
	gadget result(0x7, 32, true);
	gadget comparison = ((input)[{32 * 7, 32 * 7 +31}] == result);

	check(comparison);
}

void check_concat_extract2()
{
	gadget input = gadget(1, 16) || gadget(2, 16);
	gadget result = { 0x00010002, 32 };
	gadget comparison = (input == result);

	check(comparison);
}

void check_shr()
{
	gadget input(0x12345678, 32, false);
	gadget result(0x01234567, 32, true);
	gadget const_gadget(0x21436587, 32);
	gadget comparison = ((input >> 4)  == result);

	check(comparison);
}

void check_rotate()
{
	gadget input(0x12345678, 32, false);
	gadget result(0x81234567, 32, true);
	gadget const_gadget(0x21436587, 32);
	gadget comparison = ((input.rotate_right(32)) == input);

	check(comparison);
}

void check_and()
{
	gadget input(0x1122335E, 32, false);
	gadget result(0x1020324e, 32, true);
	gadget const_gadget(0xdeadbeef, 32);
	gadget comparison = ((input & const_gadget) == result);

	check(comparison);
}

void check_not()
{
	gadget input(0xffffffff, 32, false);
	gadget result(0x00000000, 32, true);
	gadget comparison = ((!input) == result);

	check(comparison);
}

void check_ITE()
{
	gadget first_var(0xdeadbeef, 32, false);
	gadget second_var(0x12345678, 32, false);
	gadget result(0x12345678, 32, true);
	gadget input(0, 1, true);

	gadget comparison = (ITE(input, first_var, second_var) == result);

	check(comparison);
}

void check_leq()
{
	gadget input1(1, 2, true);
	gadget input2(3, 2, true);
	gadget comparison = ((input1 <= input2) == gadget(1, 1));

	check(comparison);
}

void check_sha256()
{
	gadget input(0x33323138, 32, false);
	gadget result(0x9D21310B, 32, true);
	gadget comparison = ((sha256_gadget(input))[{224, 255}] == result);

	check(comparison);
}

#include "sha256.hpp"

void check_sha256v2()
{
	std::string input_str = "3218";
	std::string hex_digest;
	picosha2::hash256_hex_string(input_str, hex_digest);
	std::string result_str = hex_digest;

	gadget input(0x33323138, 32, true);
	gadget result(result_str, 256, true);
	gadget comparison = (sha256_gadget(input) == result);

	check(comparison);
}

void check_common_prefix_mask()
{
	gadget input1(11, 4, true);
	gadget input2(7, 4, true);
	gadget result = get_common_prefix_mask(input1, input2);
	gadget comparison = (result == gadget(3, 4));

	check(comparison);
}

#include "hasher.hpp"

void test_MimC()
{
	gadget input(0xdeadbeef, 32, true);
	using hasher = merkle_tree::MimcHash<PRIME_NUM, uint32_t>;
	auto result = hasher::hash_leaf(0xdeadbeef);
	gadget result_gadget(std::string("d") + result.to_string(), false);
	gadget comparison = (result_gadget == MimcLeafHash(input));

	check(comparison);
}

#include "merkle_tree.hpp"

template<uint64_t characteristics>
std::vector<gadget> convert_proof_to_gadget(const std::vector<Field<characteristics>>& x)
{
	std::vector<gadget> result;
	for (auto& elem : x)
	{
		result.emplace_back(std::string("d") + elem.to_string(), false);
	}
	return result;
}

void check_merkle_proof()
{
	std::vector<uint32_t> leaves = { 0xdeadbeef, 0x112364e1, 3, 0x12345678};
	using hasher = merkle_tree::MimcHash<PRIME_NUM, uint32_t>;
	merkle_tree::MerkleTree<hasher, uint32_t> tree(leaves);
	uint32_t raw_address = 1;
	gadget address(raw_address, tree.height(), true);
	gadget leaf(tree.get_leaf_at_address(raw_address), 32, false);
	std::vector<gadget> proof = convert_proof_to_gadget(tree.get_proof(raw_address));
	gadget merkle_root = gadget(std::string("d")+tree.get_root().to_string(), true);
	gadget flag = (merkle_tree_proof(address, leaf, proof, merkle_root, tree.height()));
	check(flag);
}


void check_transaction()
{
	std::vector<uint32_t> leaves = { 12, 43, 32, 41 };
	using hasher = merkle_tree::MimcHash<PRIME_NUM, uint32_t>;
	merkle_tree::MerkleTree<hasher, uint32_t> tree(leaves);
	uint32_t raw_from_address = 1;
	uint32_t raw_to_address = 3;
	uint32_t raw_amount = 9;

	gadget merkle_root_before = gadget(std::string("d") + tree.get_root().to_string(), true);

	gadget from_address(raw_from_address, tree.height(), true);
	gadget from_balance(tree.get_leaf_at_address(raw_from_address), 32, false);
	std::vector<gadget> from_proof_before = convert_proof_to_gadget(tree.get_proof(raw_from_address));
	
	gadget to_address(raw_to_address, tree.height(), true);
	gadget to_balance(tree.get_leaf_at_address(raw_to_address), 32, false);
	std::vector<gadget> to_proof_before = convert_proof_to_gadget(tree.get_proof(raw_to_address));

	tree.update(raw_from_address, raw_to_address, raw_amount);

	gadget amount(raw_amount, 32, true);
	std::vector<gadget> from_proof_after = convert_proof_to_gadget(tree.get_proof(raw_from_address));
	std::vector<gadget> to_proof_after = convert_proof_to_gadget(tree.get_proof(raw_to_address));
	gadget merkle_root_after = gadget(std::string("d") + tree.get_root().to_string(), true);

	gadget flag = check_transaction(from_address, to_address, from_balance, to_balance, amount,
		merkle_root_before, merkle_root_after, from_proof_before, to_proof_before,
		from_proof_after, to_proof_after);

	check(flag);
}


void test_all()
{
	//check_addition();
	
	//check_concat_extract();
	//check_concat_extract2();
	//check_shr();
	//check_rotate();		
	//check_ITE();
	//check_leq();
	//check_addition_xor();
	//check_and();
	//check_not();
	//check_sha256();
	//check_sha256v2();
	//check_common_prefix_mask();
	//test_MimC();
	//check_merkle_proof();
	check_transaction();
}


int main(int argc, char* argv[])
{
	test_all();
	getchar();
}