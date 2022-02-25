#pragma once

#include <motion_search/inc/common.h>
#include <stdexcept>


using std::runtime_error;


class EOFException : public runtime_error
{
public:
	EOFException(void);
	~EOFException(void);
};

