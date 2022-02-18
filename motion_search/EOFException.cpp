#include "EOFException.h"


EOFException::EOFException(void) : runtime_error ("EOF Reached")
{
}


EOFException::~EOFException(void)
{
}
