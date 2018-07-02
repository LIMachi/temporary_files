#include "json.h"
#include <string.h>
#include <stdio.h>

json_cursor_t	json_increment_cursor(json_parser_data_t *data)
{
	json_cursor_t	cursor;

	cursor = data->cursor;
	if (data->source[++cursor.index] == '\n')
	{
		++cursor.line;
		cursor.column = cursor.tab_count = 0;
	}
	else
		++cursor.column;
	if (data->source[cursor.index] == '\t')
		++cursor.tab_count;
	return (cursor);
}

char *json_stracat(char *s1, char *s2)
{
	size_t	ls1;
	size_t	ls2;
	char	*out;

	ls1 = strlen(s1);
	ls2 = strlen(s2);
	if ((out = realloc(s1, ls1 + ls2 + 1)) == NULL)
		return (NULL);
	memmove(&out[ls1], s2, ls2);
	out[ls1 + ls2] = '\0';
	return (out);
}

json_node_t		*json_new_node(json_parser_data_t *data, json_type_t type, json_node_t *parent)
{
	json_node_t	*out;

	if (NULL == (out = malloc(sizeof(json_node_t))))
		return (NULL);
	out->parent = parent;
	out->next = NULL;
	out->type = type;
	out->start = data->cursor;
	switch (type)
	{
		case JSON_OBJECT: case JSON_ARRAY:
			out->shild_count = 0;
			out->node = NULL;
		break;
		case JSON_PAIR:
			out->key = NULL;
			out->value = NULL;
		break;
		case JSON_INT:
			out->integer = 0;
		break;
		case JSON_FLOAT:
			out->floating = 0.0;
	}
	return (out);
}

json_status_t	json_parse_string(json_parser_data_t *data)
{

}

json_status_t	json_validate_node(json_node_t *node, json_type_t type, int mode)
{
	if (node == NULL || (mode == 2 ? (node->type != type) : (node->type != JSON_UNDEFINED) || node->start.index == -1 || (node->end.index == -1 && !mode))
		return (JSON_PINVAL);
	return (JSON_CONTINUE);
}

json_status_t	json_error(json_parser_data_t *data, const char *msg, json_status_t code)
{
	json_cursor_t	cursor;
	size_t			endline;
	size_t			startline;
	char			*file_name;

	file_name = data->file_name;
	if (file_name == NULL)
		file_name = "<unnamed file>";
	cursor = data->cursor;
	startline = cursor.index - cursor.column;
	for (endline = cursor.index; data->source[endline] != '\0' && data->source[endline] != '\n'; ++endline);
	printf("[json parser][ERROR][%d]: %s\n", code, msg);
	size_t	t = printf("[%s][%d][%d][%d]: ", file_name, cursor.line, cursor.column,
			cursor.column + cursor.tab_count * (D_TAB_LENGTH - 1));
	printf("%.*s\n", endline - startline, &data->source[startline]);
	t += cursor.column + cursor.tab_count * (D_TAB_LENGTH - 1);
	for (; t; --t)
		printf(" ");
	printf("^\n");
	return (code);
}

json_status_t	json_parse(json_parser_data_t *data, char *source, json_flags_t flags)
{
	json_status_t	ret;

	if (data == NULL)
		return (JSON_PINVAL);
	if (source != NULL)
	{
		if (data->source == NULL)
			if (flags & JSON_FLAG_STATIC_SOURCE)
				data->source = strdup(source);
			else
				data->source = source;
		else
			data->source = json_stracat(data->source, source);
	}
	if (data->source == NULL)
		return (JSON_PINVAL);
	for (; data->source[data->cursor.index] != '\0'; data->cursor = json_increment_cursor(data))
	{
		char c = data->source[data->cursor.index];

		switch (c)
		{
			case '{': case '[':
			break;
			case '}': case ']':
			break;
			case '\"':
			break;
			case ':':
				if (data->current_node != NULL)
					return (json_error(data, "unknown handling of ':' token", JSON_PINVAL));
				if (json_validate_node(data->prev_node, JSON_STRING, 1) != JSON_CONTINUE)
					return (json_error(data, "invalid ':' token after non-string token", JSON_PINVAL));
				if (json_validate_node(data->prev_node->parent, JSON_OBJECT, 0) != JSON_CONTINUE)
					return (json_error(data, "invalid ':' token in non-object token", JSON_PINVAL));
				if (NULL == (data->current_node = json_new_node(data, JSON_PAIR, data->prev_node->parent)))
					return (json_error(data, "out of memory", JSON_NOMEM));
				data->current_node->key = data->prev_node;
			break;
			case ',':
			break;
			case '-': case '.': case '0': case '1':
			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
			break;
			case 't': case 'f': case 'n':
			break;
			case '\t': case '\r': case '\n': case ' ':
			break;
			default:
			//error
			break;
		}
	}
}