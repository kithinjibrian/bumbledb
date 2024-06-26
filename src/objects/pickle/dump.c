#include "objects/pickle/pickle.h"

string_o *__dump__(object_o object, string_o *buffer)
{
	return buffer;
}

string_o *dump(object_o object)
{
	return __dump__(object, string_from(""));
}