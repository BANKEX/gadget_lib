#ifndef PROTOBOARD_HPP_
#define PROTOBOARD_HPP_

#include <vector>
#include <set>

//TODO: delete it later
#include <iostream>

namespace gadgetlib
{	
	using var_index_t = uint32_t;
	using variable_set = std::set<var_index_t>;

	template<typename FieldT>
	class protoboard;

	template<typename FieldT>
	struct pb_linear_term;

	template<typename FieldT>
	struct pb_linear_combination;

	//TODO: use BOOST operators for overloading

	template<typename FieldT>
	struct pb_variable
	{
		const var_index_t index_;

		//TODO: add implicit coversion between pb_variable, pb_linear_term, pb_linear_combination

		//pb_linear_term<FieldT> operator-() const;

		explicit pb_variable(var_index_t index) : index_(index) {}
		
	};

	template<typename FieldT>
	pb_linear_term<FieldT> operator*(const FieldT &field_coeff,
		const pb_variable<FieldT> &var)
	{
		return pb_linear_term<FieldT>{var, field_coeff};
	}

	template<typename FieldT>
	pb_linear_combination<FieldT> operator+(const pb_variable<FieldT> &var1,
		const pb_variable<FieldT> &var2)
	{
		return pb_linear_combination<FieldT>(std::initializer_list<pb_linear_term<FieldT>>{pb_linear_term<FieldT>(var1),
			pb_linear_term<FieldT>(var2)});
	}

	template<typename FieldT>
	pb_linear_combination<FieldT> operator-(const pb_variable<FieldT> &var1,
		const pb_variable<FieldT> &var2)
	{
		return pb_linear_combination<FieldT>{pb_linear_term<FieldT>(var1, 1),
			pb_linear_term<FieldT>(var2, -1)};
	}

	template<typename FieldT>
	pb_linear_combination<FieldT> operator+(const pb_linear_term<FieldT> &var1,
		const pb_linear_term<FieldT> &var2)
	{
		return pb_linear_combination<FieldT>{var1, var2};
	}

	template<typename FieldT>
	pb_linear_combination<FieldT> operator-(const pb_linear_term<FieldT>& var1,
		const pb_linear_term<FieldT>& var2)
	{
		return pb_linear_combination<FieldT>{var1, pb_linear_term<FieldT>(var2.index, -var2.coeff)};
	}

	template<typename FieldT>
	pb_linear_combination<FieldT> operator-(const pb_linear_term<FieldT>& var1,
		const pb_variable<FieldT> & var2)
	{
		return pb_linear_combination<FieldT>(std::initializer_list<pb_linear_term<FieldT>>{var1, 
			pb_linear_term<FieldT>(var2, -1)});
	}

	/*template<typename FieldT>
	pb_linear_combination<FieldT> operator+(const FieldT& field_coeff,
		const pb_variable<FieldT> &var)
	{
		return pb_linear_combination<FieldT>{}
	}

	template<typename FieldT>
	pb_linear_combination<FieldT> operator-(const FieldT &field_coeff, 
		const pb_variable<FieldT> &var);*/

	/****************************** Linear term **********************************/
	/**
	* A linear term represents a formal expression of the form "coeff * x_{index}".
	*/

	template<typename FieldT>
	struct pb_linear_term
	{
		var_index_t index;
		FieldT coeff;

		pb_linear_term() {}
		pb_linear_term(const pb_variable<FieldT> &var) : index(var.index_), coeff(1) {}
		pb_linear_term(const pb_variable<FieldT> &var, const FieldT &field_coeff) :
			index(var.index_), coeff(field_coeff) {}
		pb_linear_term(var_index_t idx, const FieldT &field_coeff) : index(idx), coeff(field_coeff) {}



		/*template<typename FieldT>
		pb_linear_term<FieldT> operator*(const FieldT &field_coeff) const;

		template<typename FieldT>
		pb_linear_combination<FieldT> operator+(
			const pb_linear_combination<FieldT> &other) const;
		template<typename FieldT>
		pb_linear_combination<FieldT> operator-(
			const pb_linear_combination<FieldT> &other) const;*/

		pb_linear_term<FieldT> operator-() const
		{
			pb_linear_term<FieldT> result;
			result.index = index;
			result.coeff = -coeff;
			return result;
		}
	};

	/*template<typename FieldT>
	pb_linear_term<FieldT> operator*(const FieldT &field_coeff, 
		const pb_linear_term<FieldT> &lt);

	template<typename FieldT>
	pb_linear_combination<FieldT> operator+(const FieldT &field_coeff, 
		const pb_linear_term<FieldT> &lt);

	template<typename FieldT>
	pb_linear_combination<FieldT> operator-(const FieldT &field_coeff, 
		const pb_linear_term<FieldT> &lt);*/


	/***************************** Linear combination ****************************/

	/**
	* A linear combination represents a formal expression of the form "sum_i coeff_i * x_{index_i}".
	*/
	template<typename FieldT>
	struct pb_linear_combination
	{
		std::vector<pb_linear_term<FieldT>> terms;

		pb_linear_combination() {};
		pb_linear_combination(const FieldT &field_coeff)
		{
			terms.emplace_back(0, field_coeff);
		}
		pb_linear_combination(const pb_variable<FieldT> &var)
		{
			terms.emplace_back(var, 1);
		}
		pb_linear_combination(const pb_linear_term<FieldT> &lt)
		{
			terms.emplace_back(lt);
		}
		pb_linear_combination(const std::initializer_list<pb_linear_term<FieldT>>& all_terms) :
			terms(all_terms) {}

		//FieldT evaluate() const;

		pb_linear_combination<FieldT> operator*(const FieldT &field_coeff) const;
		pb_linear_combination<FieldT> operator+(const pb_linear_combination<FieldT> &other) const
		{
			pb_linear_combination<FieldT> result = *this;
			result.terms.emplace_back(other.terms.begin(), other.terms.end());
			return result;
		}
		pb_linear_combination<FieldT> operator-(const pb_linear_combination<FieldT> &other)
		{
			pb_linear_combination<FieldT> result = *this;
			for (const auto& elem : other.terms)
			{
				result.terms.emplace_back(-elem);
			}
			return result;
		}

		void dump()
		{
			bool first = true;
			for (auto term : terms)
			{
				if (!first)
					std::cout << " + ";
				if (term.index > 0)
					std::cout << "(" << term.coeff << " * var_" << term.index << ") ";
				else
					std::cout << "(" <<  term.coeff << ") ";
				first = false;
			}
			std::cout << std::endl;
		}
		//pb_linear_combination<FieldT> operator-() const;
	};

	template<typename FieldT>
	pb_linear_combination<FieldT> operator+(const pb_linear_combination<FieldT>& a,
		const pb_linear_term<FieldT> b)
	{
		pb_linear_combination<FieldT> result = a;
		result.terms.emplace_back(b);
		return result;
	}

	/*template<typename FieldT>
	pb_linear_combination<FieldT> operator*(const FieldT &field_coeff, 
		const pb_linear_combination<FieldT> &lc);

	template<typename FieldT>
	pb_linear_combination<FieldT> operator+(const FieldT &field_coeff, 
		const pb_linear_combination<FieldT> &lc);

	template<typename FieldT>
	pb_linear_combination<FieldT> operator-(const FieldT &field_coeff, 
		const pb_linear_combination<FieldT> &lc);*/

	// A rank-1 constraint. The constraint is defined by
	// <a,x> * <b,x> = <c,x>
	// where x is an assignment of field elements to the variables.

	template<typename FieldT>
	using r1cs_variable_assignment = std::vector<FieldT>;
	//TODO: use boost intervals instead of simple sets
	template<typename FieldT>
	using r1cs_primary_input = std::set<var_index_t>;
	template<typename FieldT>
	using r1cs_auxiliary_input = std::set<var_index_t>;

	template<typename FieldT>
	struct r1cs_constraint
	{
		pb_linear_combination<FieldT> a_, b_, c_; // <a,x> * <b,x> = <c,x>

		r1cs_constraint(const pb_linear_combination<FieldT>& a,
			const pb_linear_combination<FieldT>& b,
			const pb_linear_combination<FieldT>& c) : a_(a), b_(b), c_(c) {}

		//bool is_satisfied(const r1cs_variable_assignment<FieldT>& assignment) const;
	};

	template<typename FieldT>
	using r1cs_constraint_system = std::vector<r1cs_constraint<FieldT>>;
	
	template<typename FieldT>
	class protoboard
	{
	public:
		//const FieldT constant_term_ = Field::one(); 
		var_index_t next_free_var_ = 1;	

		r1cs_constraint_system<FieldT> constraints_;
		//TODO: may be it is better to represent via intervals (BOOST INTERVALS?)
		variable_set public_wires;
		//TODO: assignment will be a very huge vector - how to make it smaller
		r1cs_variable_assignment<FieldT> assignment;

	public:
		protoboard()
		{
			assignment.emplace_back(1);
		};	

		void add_r1cs_constraint(const r1cs_constraint<FieldT> &constr)
		{
			constraints_.emplace_back(constr);
		}

		var_index_t pack_bits(var_index_t low, var_index_t high)
		{
			var_index_t result = get_free_var();
			uint64_t coeff = 1;
			pb_linear_combination<FieldT> eq;
			var_index_t idx = low;
			while (idx <= high)
			{
				eq = eq + pb_linear_term<FieldT>(idx, coeff);
				coeff *= 2;
				idx++;
			}
			add_r1cs_constraint({ pb_variable<FieldT>(0), eq, pb_variable<FieldT>(result) });
			return result;
		}
		
		std::pair<var_index_t, var_index_t> unpack_bits(var_index_t packed_var, uint32_t range)
		{
			auto index_range = get_free_var_range(range);
			auto idx = index_range.first;
			pb_linear_combination<FieldT> eq;
			uint64_t coeff = 1;
			while (idx <= index_range.second)
			{
				make_boolean(idx);
				eq = eq + pb_linear_term<FieldT>(idx, coeff);
				coeff *= 2;
				idx++;
			}
			add_r1cs_constraint({ pb_variable<FieldT>(0), eq, pb_variable<FieldT>(packed_var) });
			return index_range;
		}
		
		var_index_t get_free_var()
		{
			assignment.emplace_back(0);
			return next_free_var_++;
			
		}

		std::pair<var_index_t, var_index_t> get_free_var_range(uint32_t range)
		{
			var_index_t begin = next_free_var_;
			var_index_t end = next_free_var_ + range - 1;
			next_free_var_ += range;
			assignment.resize(assignment.size() + range);
			return std::make_pair(begin, end);
			
		}

		void add_public_wire(var_index_t var)
		{
			public_wires.insert(var);
		}

		void add_public_wire_range(var_index_t first, var_index_t last)
		{
			auto idx = first;
			while (idx < last)
			{
				public_wires.insert(idx);
				idx++;
			}
		}
		void make_boolean(var_index_t var)
		{
			add_r1cs_constraint({ pb_variable<FieldT>(var), 
				pb_variable<FieldT>(0) - pb_variable<FieldT>(var), FieldT(0) * pb_variable<FieldT>(0) });
		}

		FieldT compute_packed_assignment(var_index_t low, var_index_t high)
		{
			var_index_t idx = high;
			FieldT result = 0;
			while (idx >= low)
			{
				result <<= 1;
				result += assignment[idx--];
			}
			return result;
		}

		void compute_unpacked_assignment(var_index_t whole, std::pair<var_index_t, var_index_t> bits)
		{
			var_index_t start = bits.first;
			var_index_t end = bits.second;
			FieldT val = assignment[whole];
			var_index_t idx = start;
			while (idx <= end)
			{
				assignment[idx] = val % 2;
				val >>= 1;
				idx++;
			}
		}

		
	};

	template<typename FieldT>
	struct r1cs_example
	{
		r1cs_constraint_system<FieldT> constraint_system;
		r1cs_primary_input<FieldT> primary_input;
		r1cs_auxiliary_input<FieldT> auxiliary_input;
		r1cs_variable_assignment<FieldT> assignment;

		r1cs_example<FieldT>() = default;
		r1cs_example<FieldT>(const r1cs_example<FieldT> &other) = default;
		r1cs_example<FieldT>(const r1cs_constraint_system<FieldT> &constraint_system,
			const r1cs_primary_input<FieldT> &primary_input,
			const r1cs_auxiliary_input<FieldT> &auxiliary_input) :
			constraint_system(constraint_system),
			primary_input(primary_input),
			auxiliary_input(auxiliary_input)
		{};

		r1cs_example<FieldT>(r1cs_constraint_system<FieldT> &&constraint_system,
			r1cs_primary_input<FieldT> &&primary_input,
			r1cs_auxiliary_input<FieldT> &&auxiliary_input) :
			constraint_system(std::move(constraint_system)),
			primary_input(std::move(primary_input)),
			auxiliary_input(std::move(auxiliary_input))
		{};

		r1cs_example<FieldT>(const protoboard<FieldT>& pboard) :
			constraint_system(pboard.constraints_), primary_input(pboard.public_wires),
			assignment(pboard.assignment){}

		void dump()
		{
			for (auto constraint : constraint_system)
			{
				std::cout << "----------------------------------------------------------------\n";
				constraint.a_.dump();
				constraint.b_.dump();
				constraint.c_.dump();
				std::cout << eval(constraint.a_) << " * " << eval(constraint.b_) << " == " <<
					eval(constraint.c_) << std::endl;
			}
		}

		FieldT eval(const pb_linear_combination<FieldT>& elem)
		{
			FieldT val = 0;
			for (auto& term : elem.terms)
			{
				val += term.coeff * assignment[term.index];
			}
			return val;
		}

		bool check_assignment()
		{
			uint32_t counter = 0;
			for (auto& contstraint : constraint_system)
			{
				if ((eval(contstraint.a_) * eval(contstraint.b_)) != eval(contstraint.c_))
				{
					std::cout << "failed assumption: " << counter << std::endl;
					return false;
				}
				else
					counter++;
			}
			return true;
		}
	};
};

#endif // PROTOBOARD_HPP_

