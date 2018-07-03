#ifndef ANNEALING_HPP_
#define ANNEALING_HPP_

#include "gadget.hpp"
#include "protoboard.hpp"

#include <map>
#include <stack>

namespace gadgetlib
{	
	class engraver
	{
		//TODO: constructor may contain inner parameters of engraver, which enables/disables
		//additional evristics
	private:
		struct node_metadata
		{
			var_index_t packed_index = 0;
			var_index_t low_unpacked_index = 0;
			var_index_t upper_unpacked_index = 0;
			//bool overflowed = false;
			node_metadata() : packed_index(0), low_unpacked_index(0), upper_unpacked_index(0) {}
		};

		using metadata_storage = std::map<const abstract_node*, node_metadata>;

		template<typename FieldT>
		var_index_t get_packed_var(protoboard<FieldT>& pboard, metadata_storage& storage,
			abstract_node* node)
		{
			node_metadata& metadata = storage[node];
			if ((metadata.packed_index == 0) &&
				(metadata.low_unpacked_index == 0))
			{						
				metadata.packed_index = pboard.get_free_var();
				if (auto* ie = dynamic_cast<input_node*>(node))
				{
					if (ie->is_public_input_)
						pboard.add_public_wire(metadata.packed_index);
					
					pboard.assignment[metadata.packed_index] = ie->witness_;
				}
				else if (auto* ce = dynamic_cast<const_node*>(node))
				{
					pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
						pb_variable<FieldT>(metadata.packed_index),
						FieldT(ce->get_value()) * pb_variable<FieldT>(0) });

					pboard.assignment[metadata.packed_index] = ce->value_;
				}
				else
					assert(false && "No node for this type");					
			}
			else if (metadata.low_unpacked_index != 0)
			{
				metadata.packed_index = pboard.pack_bits(metadata.low_unpacked_index,
					metadata.upper_unpacked_index);

				pboard.assignment[metadata.packed_index] =
					pboard.compute_packed_assignment(metadata.low_unpacked_index,
						metadata.upper_unpacked_index);
			}
			return metadata.packed_index;
		}

		template<typename FieldT>
		std::pair<var_index_t, var_index_t> get_unpacked_var(protoboard<FieldT>& pboard, 
			metadata_storage& storage, abstract_node* node)
		{
			node_metadata& metadata = storage[node];
			if ((metadata.packed_index == 0) &&
				(metadata.low_unpacked_index == 0))
			{
				auto index_range = pboard.get_free_var_range(node->bitsize_);
				metadata.low_unpacked_index = index_range.first;
				metadata.upper_unpacked_index = index_range.second;
				if (auto* ie = dynamic_cast<input_node*>(node))
				{
					if (ie->is_public_input_)
						pboard.add_public_wire_range(index_range.first, index_range.second);
					for (auto idx = index_range.first; idx <= index_range.second; idx++)
						pboard.make_boolean(idx);

					auto val = ie->witness_;
					auto idx = index_range.first;
					auto parity_counter = 0;
					auto index = ie->char_witness_.size();
					while (idx <= index_range.second)
					{
						FieldT elem;
						if (ie->char_witness_.size() > 0)
						{
							if (parity_counter % 8 == 0)
							{
								index--;
								val = ie->char_witness_[index];
							}
							parity_counter++;
						}
						elem = val % 2;
						val >>= 1;

						pboard.assignment[idx++] = elem;
					}
				}
				else if (auto* ce = dynamic_cast<const_node*>(node))
				{
					uint32_t value = ce->get_value();
					var_index_t idx = index_range.first;
					for (unsigned i = 0; i < node->bitsize_; i++)
					{
						uint32_t bit = value % 2;
						value /= 2;
						pboard.add_r1cs_constraint({ pb_variable<FieldT>(0), pb_variable<FieldT>(idx++),
							FieldT(bit) * pb_variable<FieldT>(0) });
					}

					value = ce->get_value();
					idx = index_range.first;
					while (idx <= index_range.second)
					{
						FieldT elem = value % 2;
						value >>= 1;
						pboard.assignment[idx++] = elem;
					}
				}
				else
					assert(false && "No node for this type");
			}
			else if (metadata.packed_index != 0)
			{
				auto index_range = pboard.unpack_bits(metadata.packed_index, node->bitsize_);
				metadata.low_unpacked_index = index_range.first;
				metadata.upper_unpacked_index = index_range.second;
				pboard.compute_unpacked_assignment(metadata.packed_index, index_range);
			}
			return std::make_pair(metadata.low_unpacked_index, metadata.upper_unpacked_index);
		}

		template<typename FieldT>
		void make_logical_constraint(protoboard<FieldT>& pboard, OP_KIND operation, var_index_t a,
			var_index_t b, var_index_t c)
		{
			switch (operation)
			{
			case (OP_KIND::CONJUNCTION):
			{
				pboard.add_r1cs_constraint({ pb_variable<FieldT>(a), 
					pb_variable<FieldT>(b), pb_variable<FieldT>(c) });
				pboard.assignment[c] = pboard.assignment[a] & pboard.assignment[b];
				break;			
			}
			case (OP_KIND::XOR):
			{
				pboard.add_r1cs_constraint({ FieldT(2) * pb_variable<FieldT>(a), pb_variable<FieldT>(b),
					pb_variable<FieldT>(a) + pb_variable<FieldT>(b) - pb_variable<FieldT>(c) });
				pboard.assignment[c] = pboard.assignment[a] ^ pboard.assignment[b];
				break;
			}
			case (OP_KIND::DISJUNCTION):
			{
				pboard.add_r1cs_constraint({ FieldT(1) * pb_variable<FieldT>(0) - pb_variable<FieldT>(a),
					FieldT(1) * pb_variable<FieldT>(0) - pb_variable<FieldT>(b),
					FieldT(1) * pb_variable<FieldT>(0) - pb_variable<FieldT>(c) });
				pboard.assignment[c] = pboard.assignment[a] | pboard.assignment[b];
				break;
			}
			default:
			{
				assert(false &&  "incorrect operation");
				break;
			}
			}
		}
	public:
		template<typename FieldT>
		void incorporate_gadget(protoboard<FieldT>& pboard, const gadget& g)
		{					
			struct vertex
			{
				const op_node* g_ptr_;
				uint32_t childs_processed_;
				vertex(const op_node* g_ptr) : g_ptr_(g_ptr), childs_processed_(0) {};
			};

			std::stack<vertex> vertexes;
			std::set<const op_node*> processed_nodes;
			metadata_storage storage;
			assert(g.kind_ == NODE_KIND::OPERATION_GADGET);
			vertexes.push(dynamic_cast<const op_node*>(g.node_.get()));

			while (vertexes.size() > 0)
			{
				vertex& e = vertexes.top();
				if ((processed_nodes.find(e.g_ptr_) == processed_nodes.end()) &&
					(e.childs_processed_ < e.g_ptr_->get_num_of_children()))
				{
					auto* child = e.g_ptr_->get_child(e.childs_processed_);
					if (auto op_child = dynamic_cast<op_node*>(child))
						vertexes.push(op_child);
					e.childs_processed_++;
				}
				else
				{
					if (processed_nodes.find(e.g_ptr_) != processed_nodes.end())
					{
						vertexes.pop();
						continue;
					}
					processed_nodes.insert(e.g_ptr_);
					switch (e.g_ptr_->kind())
					{
					case (OP_KIND::PLUS):					
					{
						auto* first_child = e.g_ptr_->get_child(0);
						auto* second_child = e.g_ptr_->get_child(1);
						var_index_t first_index = get_packed_var(pboard, storage, first_child);
						var_index_t second_index = get_packed_var(pboard, storage, second_child);
						var_index_t result_index = pboard.get_free_var();

						pboard.add_r1cs_constraint({ pb_variable<FieldT>(0), 
							pb_variable<FieldT>(first_index) + pb_variable<FieldT>(second_index), 
							pb_variable<FieldT>(result_index) });

						pboard.assignment[result_index] = pboard.assignment[first_index] + 
							pboard.assignment[second_index];

						auto index_range = pboard.unpack_bits(result_index, e.g_ptr_->bitsize_ + 1);
						pboard.compute_unpacked_assignment(result_index, index_range);
						
						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = index_range.first;
						metadata.upper_unpacked_index = index_range.second - 1;
						
						break;
					}
					case (OP_KIND::CONJUNCTION):
					case (OP_KIND::XOR):
					case (OP_KIND::DISJUNCTION):
					{
						auto* first_child = e.g_ptr_->get_child(0);
						auto* second_child = e.g_ptr_->get_child(1);
						auto first_index_range = get_unpacked_var(pboard, storage, first_child);
						auto second_index_range = get_unpacked_var(pboard, storage, second_child);
						auto final_index_range = pboard.get_free_var_range(e.g_ptr_->bitsize_);

						for (unsigned i = 0; i < e.g_ptr_->bitsize_; i++)
						{
							make_logical_constraint(pboard, e.g_ptr_->kind(),
								first_index_range.first + i, second_index_range.first + i,
								final_index_range.first + i);
						}

						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = final_index_range.first;
						metadata.upper_unpacked_index = final_index_range.second;

						break;
					}
					case (OP_KIND::EQ):
					{
						auto* first_child = e.g_ptr_->get_child(0);
						auto* second_child = e.g_ptr_->get_child(1);
						auto first_index_range = get_unpacked_var(pboard, storage, first_child);
						auto second_index_range = get_unpacked_var(pboard, storage, second_child);

						for (unsigned i = 0; i < first_child->bitsize_; i++)
						{
							pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
								pb_variable<FieldT>(first_index_range.first + i),
								pb_variable<FieldT>(second_index_range.first + i) });
						}

						break;
					}					
					case (OP_KIND::INDEX):
					{
						auto* child = e.g_ptr_->get_child(0);
						auto index_range = get_unpacked_var(pboard, storage, child);
						uint32_t ub = e.g_ptr_->additional_param_;
						uint32_t lb = e.g_ptr_->param_;
						uint32_t final_length = ub - lb + 1;
						auto final_index_range  = pboard.get_free_var_range(final_length);
						uint32_t start = child->bitsize_ - ub - 1;
						
						for (unsigned i = 0; i < final_length; i++)
						{
							pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
								pb_variable<FieldT>(index_range.first + start + i),
								pb_variable<FieldT>(final_index_range.first + i) });

							pboard.assignment[final_index_range.first + i] = 
								pboard.assignment[index_range.first + start + i];
						}

						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = final_index_range.first;
						metadata.upper_unpacked_index = final_index_range.second;

						break;
					}
				
					case (OP_KIND::SHR):
					{
						auto* child = e.g_ptr_->get_child(0);
						uint32_t shift = e.g_ptr_->param_;
						auto index_range = get_unpacked_var(pboard, storage, child);
						auto final_index_range = pboard.get_free_var_range(e.g_ptr_->bitsize_);

						for (unsigned i = 0; i < e.g_ptr_->bitsize_; i++)
						{
							auto j = index_range.first + i + shift;
							if (j <= index_range.second)
							{
								pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
									pb_variable<FieldT>(j),
									pb_variable<FieldT>(final_index_range.first + i) });

								pboard.assignment[final_index_range.first + i] =
									pboard.assignment[j];
							}
							else
							{
								pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
									pb_variable<FieldT>(final_index_range.first + i),
									FieldT(0) * pb_variable<FieldT>(0) });

								pboard.assignment[final_index_range.first + i] = 0;
							}
						}

						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = final_index_range.first;
						metadata.upper_unpacked_index = final_index_range.second;
						
						break;
					}
					case (OP_KIND::NOT):
					{
						auto* child = e.g_ptr_->get_child(0);
						auto index_range = get_unpacked_var(pboard, storage, child);

						auto final_index_range = pboard.get_free_var_range(e.g_ptr_->bitsize_);

						for (unsigned i = 0; i < e.g_ptr_->bitsize_; i++)
						{
							pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
								pb_variable<FieldT>(1) - pb_variable<FieldT>(index_range.first + i),
								pb_variable<FieldT>(final_index_range.first + i) });

							pboard.assignment[final_index_range.first + i] =
								1 - pboard.assignment[index_range.first + i];
						}

						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = final_index_range.first;
						metadata.upper_unpacked_index = final_index_range.second;
						break;
					}
					case (OP_KIND::ROTATE_LEFT):
					case (OP_KIND::ROTATE_RIGHT):
					{
						auto* child = e.g_ptr_->get_child(0);
						auto index_range = get_unpacked_var(pboard, storage, child);
						uint32_t shift = (e.g_ptr_->kind() == OP_KIND::ROTATE_RIGHT ? e.g_ptr_->param_ :
							 e.g_ptr_->bitsize_ - e.g_ptr_->param_);
						auto final_index_range = pboard.get_free_var_range(e.g_ptr_->bitsize_);
						auto k = index_range.first;

						for (unsigned i = 0; i < e.g_ptr_->bitsize_; i++)
						{
							auto j = index_range.first + i + shift;
						
							if (j <= index_range.second)
							{
								pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
									pb_variable<FieldT>(j),
									pb_variable<FieldT>(final_index_range.first + i) });

								pboard.assignment[final_index_range.first + i] =
									pboard.assignment[j];
							}
							else
							{
								pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
									pb_variable<FieldT>(final_index_range.first + i),
									pb_variable<FieldT>(k) });

								pboard.assignment[final_index_range.first + i] =
									pboard.assignment[k++];
							}
						}

						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = final_index_range.first;
						metadata.upper_unpacked_index = final_index_range.second;
						break;
					}
					case (OP_KIND::CONCATENATION):
					{
						auto* first_child = e.g_ptr_->get_child(0);
						auto* second_child = e.g_ptr_->get_child(1);
						auto first_index_range = get_unpacked_var(pboard, storage, first_child);
						auto second_index_range = get_unpacked_var(pboard, storage, second_child);
						auto final_index_range = pboard.get_free_var_range(e.g_ptr_->bitsize_);

						for (unsigned i = 0; i < e.g_ptr_->bitsize_; i++)
						{
							if (i < second_child->bitsize_)
							{
								pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
									pb_variable<FieldT>(final_index_range.first + i),
									pb_variable<FieldT>(second_index_range.first + i) });

								pboard.assignment[final_index_range.first + i] =
									pboard.assignment[second_index_range.first + i];
							}
							else
							{
								auto j = i - second_child->bitsize_;
								pboard.add_r1cs_constraint({ pb_variable<FieldT>(0),
									pb_variable<FieldT>(final_index_range.first + i),
									pb_variable<FieldT>(first_index_range.first + j) });

								pboard.assignment[final_index_range.first + i] =
									pboard.assignment[first_index_range.first + j];
							}
						}

						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = final_index_range.first;
						metadata.upper_unpacked_index = final_index_range.second;

						break;
					}
					case (OP_KIND::ITE):
					{
						auto* condition = e.g_ptr_->get_child(0);
						auto* first_child = e.g_ptr_->get_child(1);
						auto* second_child = e.g_ptr_->get_child(2);
						auto final_index_range = pboard.get_free_var_range(e.g_ptr_->bitsize_);

						auto first_index_range = get_unpacked_var(pboard, storage, first_child);
						auto second_index_range = get_unpacked_var(pboard, storage, second_child);
						
						assert(condition->bitsize_ == 1 && "incorrect bitsize of condition");
						auto condition_index = get_unpacked_var(pboard, storage, condition).first;

						for (unsigned i = 0; i < e.g_ptr_->bitsize_; i++)
						{
							pboard.add_r1cs_constraint({ pb_variable<FieldT>(condition_index),
								pb_variable<FieldT>(first_index_range.first + i) -
								pb_variable<FieldT>(second_index_range.first + i),
								pb_variable<FieldT>(final_index_range.first + i) -
								pb_variable<FieldT>(second_index_range.first + i) });

							pboard.assignment[final_index_range.first + i] =
								pboard.assignment[condition_index] * pboard.assignment[first_index_range.first + i]
								+ (1 - pboard.assignment[condition_index]) * pboard.assignment[second_index_range.first + i];
						}

						node_metadata& metadata = storage[e.g_ptr_];
						metadata.low_unpacked_index = final_index_range.first;
						metadata.upper_unpacked_index = final_index_range.second;

						break;
					}
					}

					vertexes.pop();
				}
			}
		}
	};
}

#endif
