#ifndef MERKLE_TREE_HPP_
#define MERKLE_TREE_HPP_

#include "gadget.hpp"


namespace gadgetlib
{
	class MerkleTree
	{
		static constexpr unsigned HASH_DIGEST_SIZE = 256 / 8;

		struct MerkleBranch
		{
			std::string hash_;
			MerkleBranch* left_;
			MerkleBranch* right_;
			MerkleBranch(const std::string& hash, MerkleBranch* left, MerkleBranch* right) :
				left_(left), right_(right), hash_(hash) {}
		};
	private:
		MerkleBranch* root;

		void calculate_tree();
	public:
		std::vector<uint32_t> leaves;
		uint32_t height;
	public:
		
		void update(uint32_t from, uint32_t to, uint32_t amount);
		gadget get_root();
		std::vector<gadget> get_proof(uint32_t address);
		uint32_t get_leaf_at_address(uint32_t address);
		MerkleTree(const std::vector<uint32_t>& leaves);
	};
}

#endif




