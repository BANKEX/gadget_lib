#include "gadget.hpp"

using namespace gadgetlib;

gadget gadgetlib::operator+(const gadget& lhs, const gadget& rhs)
{
	gadget result(OP_KIND::PLUS, lhs, rhs);
	return result;
}

gadget gadgetlib::operator-(const gadget& lhs, const gadget& rhs)
{
	gadget result(OP_KIND::MINUS, lhs, rhs);
	return result;
}

gadget gadgetlib::operator||(const gadget& lhs, const gadget& rhs)
{
	return gadget(OP_KIND::CONCATENATION, lhs, rhs);
}

gadget gadgetlib::operator&(const gadget& lhs, const gadget& rhs)
{
	return gadget(OP_KIND::CONJUNCTION, lhs, rhs);
}

gadget gadgetlib::operator^(const gadget& lhs, const gadget& rhs)
{
	return gadget(OP_KIND::XOR, lhs, rhs);
}

gadget gadgetlib::operator==(const gadget& lhs, const gadget& rhs)
{
	return gadget(OP_KIND::EQ, lhs, rhs);
}

gadget gadgetlib::operator<=(const gadget& lhs, const gadget& rhs)
{
	return gadget(OP_KIND::LEQ, lhs, rhs);
}

//If-then-else construction
gadget gadgetlib::ITE(const gadget& condition, const gadget& first_choice, 
	const gadget& second_choice)
{
	return gadget(condition, first_choice, second_choice);
}

fgadget gadgetlib::operator+(const fgadget& lhs, const fgadget& rhs)
{
	fgadget result(OP_KIND::FADD, lhs, rhs);
	return result;
}

fgadget gadgetlib::operator*(const fgadget& lhs, const fgadget& rhs)
{
	fgadget result(OP_KIND::FMUL, lhs, rhs);
	return result;
}

