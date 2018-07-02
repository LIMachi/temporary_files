#include "json.h"
#include <stdio.h>

int main()
{
	json_parser_data_t	data;

	data = JSON_PARSER_INIT;

	json_parse(&data, "\"test\":\"truc\"", JSON_FLAG_STATIC_SOURCE);
}