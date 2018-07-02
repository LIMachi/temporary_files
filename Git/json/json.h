#include <stdlib.h>

typedef enum {
	JSON_UNDEFINED = 0,
	JSON_NULL = 1,
	JSON_OBJECT = 2,
	JSON_PAIR = 3,
	JSON_ARRAY = 4,
	JSON_STRING = 5,
	JSON_FLOAT = 6,
	JSON_INT = 7,
	JSON_BOOL = 8
} json_type_t;

typedef enum {
	JSON_CONTINUE = 0,
	JSON_FINISHED = -1,
	JSON_PINVAL = -2,
	JSON_NOMEM = -3
} json_status_t;

typedef struct {
	size_t	index;
	size_t	line;
	size_t	column;
	size_t	tab_count;
} json_cursor_t;

typedef struct json_node_s	json_node_t;

struct json_node_s {
	json_node_t			*parent;
	json_node_t			*next;
	json_type_t			type;
	json_cursor_t		start;
	json_cursor_t		end;
	union {
		struct {
			size_t		shild_count;
			json_node_t	*node;
		};
		struct {
			json_node_t	*key;
			json_node_t	*value;
		};
		long long int	integer;
		double			floating;
	};
};

typedef struct {
	char			*source;
	json_node_t		*current_node;
	json_node_t		*prev_node;
	json_cursor_t	cursor;
	char			*file_name;
} json_parser_data_t;

#if !defined(NULL)
# define NULL ((void*)0)
#endif

#define JSON_CURSOR_INIT(x) ((json_cursor_t){.index = x, .line = 0, .column = 0, .tab_count = 0})

#define JSON_PARSER_INIT ((json_parser_data_t){.source = NULL, .current_node = NULL, .prev_node = NULL, .cursor = JSON_CURSOR_INIT(0), .file_name = NULL})

#define D_TAB_LENGTH 8

typedef enum {
	JSON_FLAG_NONE = 0,
	JSON_FLAG_STATIC_SOURCE = 1,
	JSON_FLAG_ALLOCATE = 2
} json_flags_t;

/*
sample code:

int main()
{
	static const char 	*source = "	{
										\"test\": 3.14,
										\"array\": [1,
										true]
									}";
	json_parser_data_t	data = JSON_PARSER_INIT;
	if (JSON_FINISHED != json_parse(&data, source, 0, JSON_FLAG_STATIC_SOURCE | JSON_FLAG_ALLOCATE))
		printf("error\n");
}
*/

//note: all string (static or allocated) passed to this function
//will be handled internally and should not be modified or freed after call to this function
//any new string sent will by concatenated to the last string sent

json_status_t	json_parse(json_parser_data_t *data, char *source, json_flags_t flags);