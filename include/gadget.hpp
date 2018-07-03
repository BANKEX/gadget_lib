#ifndef GADGET_HPP_
#define GADGET_HPP_

//TODO: apply BOOST proto - what is used is a rough description
#include <stdint.h>

#include <memory>
#include <cassert>
#include <vector>

namespace gadgetlib
{
	struct range
	{
		uint32_t lower_bound_, upper_bound_;
		range(uint32_t lower_bound, uint32_t upper_bound) : lower_bound_((uint32_t)lower_bound),
			upper_bound_((uint32_t)upper_bound) {}
	};

	class gadget;

	class abstract_node
	{
	protected:
		abstract_node(uint32_t bitsize = 0) : bitsize_(bitsize) {};

		friend class gadget;
	public:
		uint32_t bitsize_;
		virtual ~abstract_node() = default;		
	};

	enum class NODE_KIND : uint8_t
	{
		OPERATION_GADGET,
		INPUT_GADGET,
		CONSTANT_GADGET,
		UNDEFINED
	};

	enum class OP_KIND : uint8_t
	{
		PLUS,
		MINUS,
		INDEX,
		CONJUNCTION,
		XOR,
		DISJUNCTION,
		SHR,
		NOT,
		ROTATE_LEFT,
		ROTATE_RIGHT,
		CONCATENATION,
		ITE,
		EQ,
		MUL,
		LEQ,
		ZEXT,
		//raw field operations
		FADD,
		FMUL
	};

	class op_node : public abstract_node
	{
	private:
		friend class gadget;
	public:
		OP_KIND op_kind_;
		std::shared_ptr<abstract_node> first_child_;
		std::shared_ptr<abstract_node> second_child_;
		//TODO: optimize it later
		std::shared_ptr<abstract_node> third_child_ = nullptr;
		uint32_t param_;
		uint32_t additional_param_;

		op_node(OP_KIND op_kind, std::shared_ptr<abstract_node> first_child,
			std::shared_ptr<abstract_node> second_child = nullptr) : op_kind_(op_kind), 
			first_child_(first_child), second_child_(second_child) 
		{
			if (op_kind == OP_KIND::CONCATENATION)
				bitsize_ = first_child->bitsize_ + second_child->bitsize_;
			else if (op_kind == OP_KIND::EQ)
				bitsize_ = 1;
			else
			{
				assert(first_child->bitsize_ == second_child->bitsize_);
				bitsize_ = first_child->bitsize_;
			}
		};

		op_node(std::shared_ptr<abstract_node> first_child,
			std::shared_ptr<abstract_node> second_child,
			std::shared_ptr<abstract_node> third_child) : op_kind_(OP_KIND::ITE),
			first_child_(first_child), second_child_(second_child),
			third_child_(third_child)
		{
			bitsize_ = second_child->bitsize_;
		}

		op_node(std::shared_ptr<abstract_node> child, uint32_t param, uint32_t additional_param):
			op_kind_(OP_KIND::INDEX), first_child_(child), second_child_(nullptr),
			param_(param), additional_param_(additional_param)
		{
			bitsize_ = additional_param - param + 1;
		}

		op_node(OP_KIND op_kind, std::shared_ptr<abstract_node> child,
			uint32_t param) : op_kind_(op_kind), first_child_(child), 
			second_child_(nullptr), param_(param)
		{
			if (op_kind == OP_KIND::ZEXT)
				bitsize_ = param;
			bitsize_ = child->bitsize_;
		};

		unsigned get_num_of_children() const
		{
			if (op_kind_ == OP_KIND::ITE)
				return 3;
			return (second_child_ ? 2 : 1);
		}

		abstract_node* get_child(uint32_t index) const
		{
			assert(index < 3);
			if (index == 0)
				return first_child_.get();
			else if (index == 1)
				return second_child_.get();
			return third_child_.get();
		}

		OP_KIND kind() const { return op_kind_; }
	};

	class input_node : public abstract_node
	{
	public:
		bool is_public_input_;
		uint32_t witness_;
		std::vector<unsigned char> char_witness_;
	public:
		input_node(uint32_t bitsize, bool is_public_input = false, uint32_t witness = 0) : 
			abstract_node(bitsize), is_public_input_(is_public_input), witness_(witness) {}

		input_node(uint32_t bitsize, bool is_public_input, 
			std::vector<unsigned char>& witness) :
			abstract_node(bitsize), is_public_input_(is_public_input), 
			char_witness_(witness) {}

		friend class gadget;
	};

	class const_node : public abstract_node
	{
	public:
		uint32_t value_;
		std::string str_value_;

		friend class gadget;
	public:
		const_node(uint32_t value, uint32_t bitlength) :
			value_(value), abstract_node(bitlength) {}
		uint32_t get_value()
		{
			return value_;
		}
	};

	class gadget
	{
	public:
		std::shared_ptr<abstract_node> node_;
		NODE_KIND kind_;

	public:
		gadget(std::uint32_t value, std::uint32_t bitlength) :
			node_(std::make_shared<const_node>(value, bitlength)),
			kind_(NODE_KIND::CONSTANT_GADGET) {}
		gadget() : kind_(NODE_KIND::UNDEFINED) {}
		gadget(OP_KIND op_kind, const gadget& first_child, const gadget& second_child) :
			node_(std::make_shared<op_node>(op_kind, first_child.node_, second_child.node_)),
			kind_(NODE_KIND::OPERATION_GADGET) {}
		gadget(OP_KIND op_kind, const gadget& child, uint32_t param = 0) :
			node_(std::make_shared<op_node>(op_kind, child.node_, param)),
			kind_(NODE_KIND::OPERATION_GADGET) {}
		gadget(uint32_t bitsize, bool is_public_input = false, uint32_t witness = 0) :
			node_(std::make_shared<input_node>(bitsize, is_public_input, witness)),
			kind_(NODE_KIND::INPUT_GADGET) {}
		gadget(std::vector<unsigned char> arr, uint32_t bitsize,
			bool is_public_input):
			node_(std::make_shared<input_node>(bitsize, is_public_input, arr)),
			kind_(NODE_KIND::INPUT_GADGET) {}
		gadget(const gadget& child, uint32_t lower_bound, uint32_t upper_bound) :
			node_(std::make_shared<op_node>(child.node_, lower_bound, upper_bound)),
			kind_(NODE_KIND::OPERATION_GADGET) {}
		gadget(const gadget& first_child, const gadget& second_child, 
			const gadget& third_child) :
			node_(std::make_shared<op_node>(first_child.node_, second_child.node_,
				third_child.node_)),
			kind_(NODE_KIND::OPERATION_GADGET) {}

		uint32_t get_bitsize() const { return node_->bitsize_; } 
		
		gadget operator[](range range) 
		{
			return gadget(*this, range.lower_bound_, range.upper_bound_);
		}
		
		gadget operator>>(uint32_t shift_len)
		{
			return gadget(OP_KIND::SHR, *this, shift_len);
		}
		
		gadget operator!()
		{
			return gadget(OP_KIND::NOT, *this);
		}

		gadget rotate_left(uint32_t shift_len)
		{
			return gadget(OP_KIND::ROTATE_LEFT, *this, shift_len);
		}

		gadget rotate_right(uint32_t shift_len)
		{
			return gadget(OP_KIND::ROTATE_RIGHT, *this, shift_len);
		}

		gadget zext(uint32_t new_size)
		{
			return gadget(OP_KIND::ZEXT, *this, new_size);
		}
	};

	gadget operator+(const gadget& lhs, const gadget& rhs);
	gadget operator-(const gadget& lhs, const gadget& rhs);
	gadget operator||(const gadget& lhs, const gadget& rhs);
	gadget operator&(const gadget& lhs, const gadget& rhs);
	gadget operator^(const gadget& lhs, const gadget& rhs);
	gadget operator==(const gadget& lhs, const gadget& rhs);
	gadget operator<=(const gadget& lhs, const gadget& rhs);

	//If-then-else construction
	gadget ITE(const gadget& condition, const gadget& first_choice,
		const gadget& second_choice);

	class fgadget
	{
	public:
		std::shared_ptr<abstract_node> node_;
		NODE_KIND kind_;
	public:
		fgadget() : kind_(NODE_KIND::UNDEFINED) {}
		fgadget(OP_KIND op_kind, const fgadget& first_child, 
			const fgadget& second_child) :
			node_(std::make_shared<op_node>(op_kind, first_child.node_, 
				second_child.node_)), kind_(NODE_KIND::OPERATION_GADGET) {}
		fgadget(uint32_t num) :
			node_(std::make_shared<const_node>(num, 0)),
			kind_(NODE_KIND::CONSTANT_GADGET) {}
		uint32_t get_bitsize() const { return node_->bitsize_; }
	};

	fgadget operator+(const fgadget& lhs, const fgadget& rhs);

	fgadget operator*(const fgadget& lhs, const fgadget& rhs);
}

#endif


