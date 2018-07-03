#ifndef BASIC_GADGETS_HPP_
#define BASIC_GADGETS_HPP_

#include "merkle_tree.hpp"

#include <vector>

#include <boost/multiprecision/cpp_int.hpp>

namespace gadgetlib
{
	//constructs output from input - linearly as it is done by standard sha256
	gadget sha256_gadget(gadget message)
	{
		auto bitsize = message.get_bitsize();

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
		uint32_t temp_len = (message.get_bitsize() + 1) % 512;
		uint32_t padding_len = ((temp_len <= 448) ? 448 - temp_len :
			448 + (512 - temp_len));
		gadget padded_message = message || gadget(1, (uint32_t)1);
		while (padding_len > 0)
		{
			if (padding_len >= 32)
			{
				padded_message = padded_message || gadget(0, (uint32_t)32);
				padding_len -= 32;
			}
			else
			{
				padded_message = padded_message || gadget(0, padding_len);
				padding_len = 0;
			}
		}

		padded_message = padded_message || gadget(0, (uint32_t)32) || gadget(bitsize, (uint32_t)32);

		//gadget padded_message = message || gadget(1 << (padding_len - 1), padding_len) ||
			//gadget(bitsize, (uint32_t)64);

		uint32_t final_len = padded_message.get_bitsize();

		for (unsigned index = 0; index < final_len / 512; index++)
		{
			gadget w[64];
			for (auto i = 0; i < 16; i++)
				w[i] = padded_message[{512 * index + i * 32, 512 * index + i * 32 + 31}];

			for (auto i = 16; i <= 63; i++)
			{
				gadget s0 = w[i - 15].rotate_right(7) ^ w[i - 15].rotate_right(18) ^
					(w[i - 15] >> 3);
				gadget s1 = w[i - 2].rotate_right(17) ^ w[i - 2].rotate_right(19) ^
					(w[i - 2] >> 10);
				w[i] = w[i - 16] + s0 + w[i - 7] + s1;
			}

			//Initialization of auxilary variables :
			gadget a = h0;
			gadget b = h1;
			gadget c = h2;
			gadget d = h3;
			gadget e = h4;
			gadget f = h5;
			gadget g = h6;
			gadget h = h7;

			//The main cycle:
			for (auto i = 0; i < 64; i++)
			{
				gadget sigma0 = a.rotate_right(2) ^ a.rotate_right(13) ^ a.rotate_right(22);
				gadget Ma = (a & b) ^ (a & c) ^ (b & c);
				gadget t2 = sigma0 + Ma;
				gadget sigma1 = e.rotate_right(6) ^ e.rotate_right(11) ^ e.rotate_right(25);
				gadget Ch = (e & f) ^ ((!e) & g);
				gadget t1 = h + sigma1 + Ch + gadget(k_arr[i], (uint32_t)32) + w[i];


				h = g;
				g = f;
				f = e;
				e = d + t1;
				d = c;
				c = b;
				b = a;
				a = t1 + t2;
			}

			h0 = h0 + a;
			h1 = h1 + b;
			h2 = h2 + c;
			h3 = h3 + d;
			h4 = h4 + e;
			h5 = h5 + f;
			h6 = h6 + g;
			h7 = h7 + h;
		}

		gadget digest = h0 || h1 || h2 || h3 || h4 || h5 || h6 || h7;
		return digest;
	}

	//something that simply converts strings to 256 bit hashes
	gadget stub_hash(gadget message)
	{
		auto bitsize = message.get_bitsize();

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
		uint32_t temp_len = (message.get_bitsize() + 1) % 512;
		uint32_t padding_len = ((temp_len <= 448) ? 448 - temp_len :
			448 + (512 - temp_len));
		gadget padded_message = message || gadget(1, (uint32_t)1);
		while (padding_len > 0)
		{
			if (padding_len >= 32)
			{
				padded_message = padded_message || gadget(0, (uint32_t)32);
				padding_len -= 32;
			}
			else
			{
				padded_message = padded_message || gadget(0, padding_len);
				padding_len = 0;
			}
		}

		padded_message = padded_message || gadget(0, (uint32_t)32) || gadget(bitsize, (uint32_t)32);

		//gadget padded_message = message || gadget(1 << (padding_len - 1), padding_len) ||
		//gadget(bitsize, (uint32_t)64);

		uint32_t final_len = padded_message.get_bitsize();

		for (unsigned index = 0; index < final_len / 512; index++)
		{
			gadget w[64];
			for (auto i = 0; i < 16; i++)
				w[i] = padded_message[{512 * index + i * 32, 512 * index + i * 32 + 31}];

			for (auto i = 16; i <= 63; i++)
			{
				gadget s0 = w[i - 15].rotate_right(7) ^ w[i - 15].rotate_right(18) ^
					(w[i - 15] >> 3);
				gadget s1 = w[i - 2].rotate_right(17) ^ w[i - 2].rotate_right(19) ^
					(w[i - 2] >> 10);
				w[i] = w[i - 16] + s0 + w[i - 7] + s1;
			}

			//Initialization of auxilary variables :
			gadget a = h0;
			gadget b = h1;
			gadget c = h2;
			gadget d = h3;
			gadget e = h4;
			gadget f = h5;
			gadget g = h6;
			gadget h = h7;

			//The main cycle:
			for (auto i = 0; i < 1; i++)
			{
				gadget sigma0 = a.rotate_right(2) ^ a.rotate_right(13) ^ a.rotate_right(22);
				gadget Ma = (a & b) ^ (a & c) ^ (b & c);
				gadget t2 = sigma0 + Ma;
				gadget sigma1 = e.rotate_right(6) ^ e.rotate_right(11) ^ e.rotate_right(25);
				gadget Ch = (e & f) ^ ((!e) & g);
				gadget t1 = h + sigma1 + Ch + gadget(k_arr[i], (uint32_t)32) + w[i];


				h = g;
				g = f;
				f = e;
				e = d + t1;
				d = c;
				c = b;
				b = a;
				a = t1 + t2;
			}

			h0 = h0 + a;
			h1 = h1 + b;
			h2 = h2 + c;
			h3 = h3 + d;
			h4 = h4 + e;
			h5 = h5 + f;
			h6 = h6 + g;
			h7 = h7 + h;
		}

		gadget digest = h0 || h1 || h2 || h3 || h4 || h5 || h6 || h7;
		return digest;
	}

	gadget test_merkle_tree(gadget address, gadget leaf,
		std::vector<gadget> merkle_proof, uint32_t treeHeight)
	{
		gadget temp = leaf;
		for (uint32_t i = 0; i < treeHeight; i++)
		{
			gadget path_choice = address[{i, i}];
			temp = (ITE(path_choice, merkle_proof[i], temp)[{16, 31}]) ||
				ITE(path_choice, temp, merkle_proof[i])[{16, 31}];
		}
		return (temp);
	}

	gadget merkle_tree_proof(gadget address, gadget leaf,
		std::vector<gadget> merkle_proof, uint32_t treeHeight)
	{
		//initialize upper level gadget with root hash
		gadget temp = stub_hash(leaf);
		for (uint32_t i = 0; i < treeHeight; i++)
		{
			gadget path_choice = address[{i, i}];
			temp = stub_hash(ITE(path_choice, merkle_proof[i], temp) ||
				ITE(path_choice, temp, merkle_proof[i]));
		}
		return (temp);
	}

	gadget get_common_prefix_mask(gadget& addr1, gadget& addr2)
	{
		auto x = addr1.get_bitsize();
		auto y = addr2.get_bitsize();
		assert(addr1.get_bitsize() == addr2.get_bitsize() && "The size is not valid");
		gadget check = !(addr1 ^ addr2);
		gadget temp = { 0x1, (uint32_t)1 };
		gadget result = { 0x0, (uint32_t)32 };
		uint32_t power = 1;
		auto bitsize = addr1.get_bitsize();
		for (unsigned i = 0; i < bitsize; i++)
		{
			temp = temp & check[{bitsize - i - 1, bitsize - i - 1}];
			//TODO: make implicit size conversion
			//mul doesn't check for overflow
			result = result + ITE(temp, gadget(power, (uint32_t)32), 
				gadget(0, (uint32_t)32));
			power *= 2;
		}
		return result;
	}

	//f - stands for Field gadget
	/*fgadget pedersen_hash(fgadget a, fgadget b)
	{
		//g and h are two independent generators, we calculate and return 
		//g^a * h^b (mod p)
		static fgadget g, h;
		fgadget temp_1 = g, temp_2 = h;
		fgadget result{ a.get_bitsize(), (uint32_t)1 };
		for (unsigned i = 0; i < a.get_bitsize(); i++)
		{
			result = ITE(a[i], result * temp_1, result);
			result = ITE(b[i], result * temp_2, result);
			temp_1 = temp_1 * temp_1;
			temp_2 = temp_2 * temp_2;
		}
		return result;
	}*/

	//not entirely correct - make it smaller.
	fgadget MimcHash(fgadget a, fgadget b)
	{
		static constexpr unsigned MIMC_ROUNDS = 322;
		//take at random - for now assume that they all will be 2;
		fgadget const_elems = 2;
		fgadget temp;

		for (unsigned i = 0; i < MIMC_ROUNDS; i++)
		{
			a = a + const_elems;
			temp = a * a;
			a = a * temp;
			a = a + b;
			b = a;
			a = b;
		}

		return a;
	}

	gadget merkle_tree_proof_pair_of_leaves(gadget addr1, gadget addr2, 
		gadget leaf1, gadget leaf2, std::vector<gadget> merkle_proof1, 
		std::vector<gadget> merkle_proof2, gadget merkle_root, 
		gadget prefix_mask, uint32_t treeHeight)
	{
		//initialize upper level gadget with root hash
		std::vector<gadget> first_hash_list, second_hash_list;
		first_hash_list.reserve(treeHeight);
		second_hash_list.reserve(treeHeight);

		first_hash_list.emplace_back(stub_hash(leaf1));
		second_hash_list.emplace_back(stub_hash(leaf2));
		for (uint32_t i = 0; i < treeHeight; i++)
		{
			gadget path_choice = addr1[{i, i}];
			gadget temp = first_hash_list.back();
			temp = sha256_gadget(ITE(path_choice, merkle_proof1[i], temp) ||
				ITE(path_choice, temp, merkle_proof1[i])); 
			first_hash_list.emplace_back(temp);
			
			gadget proof_choice = ITE(prefix_mask[{i, i}], merkle_proof1[i], 
				merkle_proof2[i]);
			path_choice = addr2[{i, i}];
			temp = second_hash_list.back();
			temp = sha256_gadget(ITE(path_choice, proof_choice, temp) ||
				ITE(path_choice, temp, proof_choice)); 
			second_hash_list.emplace_back(temp);
		}

		gadget check = ((first_hash_list.back() == merkle_root) &
			(second_hash_list.back() == merkle_root));
		first_hash_list.pop_back();
		second_hash_list.pop_back();

		gadget index = prefix_mask + gadget(1, prefix_mask.get_bitsize());

		for (uint32_t i = 0; i < treeHeight; i++)
		{
			//Here we may use more optimal variant from xJsnark paper
			//TODO: there exists more efficient construction that check & local_check &...
			//note that all flags are bits and may be packed
			gadget index_choice = index[{i, treeHeight - 1 - i}];
			gadget local_check1 = (ITE(index_choice, first_hash_list[i],
				gadget(0, (uint32_t)256)) == ITE(index_choice, merkle_proof2[i],
					gadget(0, (uint32_t)256)));
			gadget local_check2 = (ITE(index_choice, second_hash_list[i],
				gadget(0, (uint32_t)256)) == ITE(index_choice, merkle_proof1[i],
					gadget(0, (uint32_t)256)));
			check = check & local_check1 & local_check2;
		}

		return check;
	}

	//all inputs are in fact public
	gadget check_transaction(uint32_t from_address, uint32_t to_address,
		uint32_t amount_, MerkleTree& merkle_tree)
	{
		auto height = merkle_tree.height;
		gadget first_balance(32, false, merkle_tree.leaves[from_address]);
		gadget second_balance(32, false, merkle_tree.leaves[to_address]);
		gadget amount(32, false, amount_);
		gadget from_address_gadget(height, true, from_address);
		gadget to_address_gadget(height, true, to_address);

		std::vector<gadget> first_proof = 
			merkle_tree.get_proof(from_address);
		std::vector<gadget> second_proof =
			merkle_tree.get_proof(to_address);
		gadget merkle_tree_root = merkle_tree.get_root();

		gadget prefix_mask = get_common_prefix_mask(from_address_gadget, to_address_gadget);
		gadget index = prefix_mask + gadget(1, prefix_mask.get_bitsize());

		gadget proof_before_transaction = 
			merkle_tree_proof_pair_of_leaves(from_address_gadget, to_address_gadget,
			first_balance, second_balance, first_proof, second_proof, merkle_tree_root,
			prefix_mask, height);

		gadget check_spendability = (amount <= first_balance);
		merkle_tree.update(from_address, to_address, amount_);
	
		std::vector<gadget> updated_first_proof = merkle_tree.get_proof(from_address);
		std::vector<gadget> updated_second_proof = merkle_tree.get_proof(to_address);
		gadget updated_merkle_root = merkle_tree.get_root();

		//check that only one element in merkle trees have been changed
		gadget check_update_proof = (1, uint32_t(1));
		for (uint32_t i = 0; i < height; i++)
		{
			//Here we may use more optimal variant from xJsnark paper
			gadget index_choice = index[{i, i}];
			gadget local_check1 = (ITE(index_choice, first_proof[i],
				gadget(0, (uint32_t)256)) == ITE(index_choice, updated_first_proof[i],
					gadget(0, (uint32_t)256)));
			gadget local_check2 = (ITE(index_choice, second_proof[i],
				gadget(0, (uint32_t)256)) == ITE(index_choice, updated_second_proof[i],
					gadget(0, (uint32_t)256)));
			check_update_proof = check_update_proof & local_check1 & local_check2;
		}

		gadget proof_after_transaction =
			merkle_tree_proof_pair_of_leaves(from_address_gadget, to_address_gadget,
				first_balance - amount, second_balance + amount, updated_first_proof, 
				updated_second_proof, updated_merkle_root, prefix_mask, height);

		return (proof_before_transaction) & (proof_after_transaction) & check_update_proof
			& check_spendability;
	}	
}

#endif


