#define BLACKLISTED_CHAR_TAG "!\"#$%&'()*+,/;<=>?@[\\]^`{|}~ "
#define BLACKLISTED_STARTER_CHAR_TAG "-.0123456789"

xml_char_t	xml_increment_cursor(xml_root_t *root)
{
	xml_char_t	out = root->cursor;

	++out.column;
	if (root.source[++out.index] == '\n')
	{
		++out.line;
		out.column = out.horizontal_tab = 0;
	}
	else if (root.source[out.index] == '\t')
		++out.horizontal_tab;
	return (out);
}

xml_root_t	xml_parse(char *source, size_t tab_length = 4)
{
	xml_root_t root = {
		.source = NULL,
		.cursor = {
			.index = 0,
			.line = 0,
			.column = 0,
			.horizontal_tab = 0
		},
		.horizontal_tab_size = 0,
		.declaration = NULL,
		.element = NULL
	};

	root.source = source;
	root.horizontal_tab_size = tab_length;

	for (root.cursor.index = 0; root.source[root.cursor.index] != '\0'; root.cursor = xml_increment_cursor(&root))
	{
		char _c = root.source[root.cursor.index];
		if (_c == ' ' || _c == '\n' || _c == '\0' || _c == '\t' || _c == '\r')
			continue ;
		if (_c == '<')
		{

		}
	}
}