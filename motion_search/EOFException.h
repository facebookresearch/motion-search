#pragma once

#include <stdexcept>


using std::runtime_error;


class EOFException : public runtime_error
{
public:
	EOFException(void);
	~EOFException(void);
};

