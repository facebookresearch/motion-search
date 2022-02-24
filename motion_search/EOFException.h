#pragma once

#include <common.h>
#include <stdexcept>


using std::runtime_error;


class EOFException : public runtime_error
{
public:
	EOFException(void);
	~EOFException(void);
};

