//the first iteration will not accpet CDATA nor PI

typedef enum {
	none  = 0,
	empty = 3,
	start = 1,
	end   = 2
} xml_tag_type_t;

typedef enum {
	empty       = 0,
	string      = 1,
	composed    = 2,
	comment     = 3,
	declaration = 4
} xml_element_type_t;

typedef struct {
	size_t						index;
	size_t						line;
	size_t						column;
	size_t						horizontal_tab; //count the amount of horizontal tab encountered since last line break
												//a correct column value might be: column + tab * (tab_length - 1)
} xml_char_t;

typedef struct {
	xml_char_t					start;
	xml_char_t					end;
} xml_string_t;

typedef xml_string_t			xml_comment_t;

typdef struct xml_tag_s {
	xml_string_t				string;
	xml_tag_type_t				type;
	xml_attribute_t				*attribute;
	struct xml_tag_s			*next;
} xml_tag_t;

typedef struct xml_element_s {
	xml_string_t				string;
	xml_element_type_t			type;
	xml_tag_type_t				*start;
	xml_tag_type_t				*end;
	struct xml_element_s		*shild;
	struct xml_element_s		*next;
} xml_element_t;

typedef struct {
	char						*source;
	xml_char_t					cursor;
	size_t						horizontal_tab_size;
	xml_element_t				*declaration;
	xml_element_t				*element;
} xml_root_t;