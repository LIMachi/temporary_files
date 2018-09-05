#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if (!defined(NULL))
# define NULL ((void*)0)
#endif

/*
pseudo struct {
type: character/group/set/alter/meta-char
quantity: min max (ex: min 0 max -1 for *)
nb_shild:
shilds:
}
*/

/*
note: this regex will only be ASCII compatible for the firsts versions
(so character set will at most have 2^7 characters (128))
characters in set are represented via flags in 2 int64_t
*/

/*
première étape: faire un truc fonctionel sans groupe, ancres et choix
(gérer les set et les répétitions)
deuxième étape: ajouter les ancres et les choix
troisième: ajouter les groupes (sans sauvegarde)
quatrième: ajouter la sauvegarde de groupes et les références
facultatif: ajouter la gestion de classes [:name:] et le nesting de set
*/

//test1: "truc\nabcdcdz\nasd\n" -> "a[b-dz]*\n" -> {4, 12} "abcdcdz\n"

//example: only sets and repeat
// {"a", {1, 1}, "bcdz", {0, -1}, "\n", {1, 1}}

typedef enum	regex_type_e
{
	re_undefined = 0,
	re_string,
	re_set,
	re_or
}				regex_type_t;

typedef struct	regex_code_s
{
	regex_type_t			type;
	struct regex_code_s		*next; //next code, null signal end of code
	struct regex_code_s		*parent; //reference to the previous choice/group, null signal start of code
	union {
		struct {
			unsigned char	*str;
			int				str_len;
		};
		uint64_t			set[2]; //flags to all 128 ascii characters
		struct regex_code_s	*choice; //next is used for the first choice
		struct {
			int				group_index; //number used to represent this group
			int				group_flags; //unused for now, will be used to flag non capturing groups, etc...
		};
	};
	long				repeat[2]; //unused for strings
}				regex_code_t;

typedef struct	s_regex_cursor
{
	unsigned long	index;
	unsigned long	line;
	unsigned long	column;
	unsigned long	vtab_count;
}				t_regex_cursor;

typedef struct	s_regex_found
{
	t_regex_cursor	start;
	t_regex_cursor	end;
}				t_regex_found;

/*same example, with struct:
{
	.type = re_string,
	.str = "a",
	.repeat = {1, 1}
},
{
	.type = re_set,
	.str = "bcdz",
	.repeat = {0, -1}
},
{
	.type = re_string,
	.str = "\n",
	.repeat = {1, 1}
}
*/

#define REGEX_MAX_CODE_SIZE	512

typedef struct	regex_s
{
	regex_code_t	code[REGEX_MAX_CODE_SIZE];
	unsigned long	code_size;
}				regex_t;

#define REGEX_NON_REPEAT_CHAR "[$^"
#define REGEX_META_CHAR ".[{()*+?|^$"
//                       ooo  ooo
#define REGEX_ESCAPE_GROUP "wWsSdD" //word, space, digit, maj = ~
									//[A-Za-z0-9_], [\t\r\n\f ], [0-9]
#define REGEX_ESCAPE_DEFAULT "nrtab0"
#define REGEX_ESCAPE_META "dlsuwDLSUWbBAzZ123456789"

typedef enum	regex_flags_e
{
	RF_NONE = 0,
	RF_NO_NEWLINE_DOT = 1,
	RF_NO_NUL_DOT = 2
}				regex_flag_t;

static inline void	set_insert(uint64_t set[2], unsigned char c, int not)
{
	if (c > 127)
		return ;
	if (c > 63)
		if (not)
			set[1] &= ~((uint64_t)1 << (c - 64));
		else
			set[1] |= (uint64_t)1 << (c - 64);
	else
		if (not)
			set[0] &= ~((uint64_t)1 << c);
		else
			set[0] |= (uint64_t)1 << c;
}

typedef enum	e_unescape_code
{
	re_unescape_uknown = -1,				// example: '\0' after '\\'
	re_unescape_whitespaces = -2,			// \s
	re_unescape_nonwhitespaces = -3,		// \S
	re_unescape_digits = -4,				// \d
	re_unescape_nondigits = -5,				// \D
	re_unescape_wordchars = -6,				// \w
	re_unescape_nonwordchars = -7,			// \W
	re_unescape_wordboundary = -8,			// \b (only out of set)
	re_unescape_nonwordboundary = -9,		// \B
	re_unescape_wordstart = -10,			// \<
	re_unescape_wordend = -11,				// \>
	re_unescape_groupreference = -12,		// \v (where v is a number > 0)
	re_unescape_startofstring = -13,		// \A
	re_unescape_endofstring = -14			// \Z
}				t_unsecape_code;

typedef enum	e_unescape_flags
{
	re_unescape_inset = 1
}				t_unsecape_flags;

// /xVV == hexa VV
// /j == j

static inline unsigned char	unescape(unsigned char *str, t_unsecape_flags flags, int *index)
{
	int r;
	char *tmp;

	if (*str != '\\')
		return (*str);
	if (index == NULL)
		index = (int[1]){1};
	else
		*index = 1;
	switch (str[(*index)++])
	{
		case 0: return (re_unescape_uknown);
		case 's': return (re_unescape_whitespaces);
		case 'S': return (re_unescape_nonwhitespaces);
		case 'd': return (re_unescape_digits);
		case 'D': return (re_unescape_nondigits);
		case 'w': return (re_unescape_wordchars);
		case 'W': return (re_unescape_nonwordchars);
		case 'b': return (flags & re_unescape_inset ? '\b' : re_unescape_wordboundary);
		case 'B': return (re_unescape_nonwordboundary);
		case '<': return (re_unescape_wordstart);
		case '>': return (re_unescape_wordend);
		case '1': case '2': case '3': case '4': case '5': case '6':
		case '7': case '8': case '9': return (re_unescape_groupreference);
		case 'A': return (re_unescape_startofstring);
		case 'Z': return (re_unescape_endofstring);
		case 'n': return ('\n');
		case 'r': return ('\r');
		case 't': return ('\t');
		case '0': return ('\0');
		case 'v': return ('\v');
		case 'f': return ('\f');
		case 'a': return ('\a');
		case 'x':
			r = strtol(&str[*index], &tmp, 16);
			if ((size_t)tmp - (size_t)&str[*index] != 2 || r > 127)
				return (-1);
				// error() invalid hexadecimal sequence after /x
				NULL;
			*index += 2;
			return (r);
		break;
		default: return (str[*index - 1]);
	}
}

int	regex_set(unsigned char *exp, uint64_t set[2], int not)
{
	unsigned char	prev_char = 0xFF;
	uint64_t		word_set[2] = {(0x3FFFFFFull << ('A' - 64)) | (0x3FFFFFFull << ('a' - 64)) | (1ull << '_' - 64), 0};
	uint64_t		space_set[2] = {0, 0x8003600ull};
	uint64_t		digit_set[2] = {0, 0x3FFull << '0'};
	int				i;
	int				t;
	char			c;

	for (i = 0; exp[i] != ']' && exp[i] != '\0'; ++i)
	{
		switch (exp[i])
		{
			case '\\':
				if ((c = unescape(exp + i, re_unescape_inset, &t)) < 0)
				{
					if (c == re_unescape_uknown)
						// error() escaped null {'\\', '\0'}
						NULL;
					i += t;
					prev_char = 0xFE;
					if (not)
						continue;
					switch (c)
					{
						case re_unescape_digits: case re_unescape_nondigits:
							set[0] |= c == re_unescape_digits ? digit_set[0] : ~digit_set[0];
							set[1] |= c == re_unescape_digits ? digit_set[1] : ~digit_set[1];
						break;
						case re_unescape_whitespaces: case re_unescape_nonwhitespaces:
							set[0] |= c == re_unescape_whitespaces ? space_set[0] : ~space_set[0];
							set[1] |= c == re_unescape_whitespaces ? space_set[1] : ~space_set[1];
						break;
						case re_unescape_wordchars: case re_unescape_nonwordchars:
							set[0] |= c == re_unescape_wordchars ? word_set[0] : ~word_set[0];
							set[1] |= c == re_unescape_wordchars ? word_set[1] : ~word_set[1];
						break;
					}
					continue;
				}
				i += t;
				set_insert(set, c, not);
				prev_char = c;
				continue;
			continue;
			case '-':
				if (exp[i + 1] == ']' || prev_char == 0xFF)
					set_insert(set, '-', not);
				else if (prev_char == 0xFE)
					continue;
				else if (prev_char > exp[++i])
					return (-1); //error
				else
					for (int j = prev_char + 1; j <= exp[i]; ++j)
						set_insert(set, j, not);
				prev_char = 0xFE;
			continue;
			default:
				set_insert(set, exp[i], not);
				prev_char = exp[i];
		}
	}
	return (i);
}

void	regex_debug_print_code(regex_t *reg)
{
	regex_code_t	*cc;

	if (reg == NULL)
		return ;
	printf("code size: %lu\n", reg->code_size);
	for (int i = 0; i < reg->code_size; ++i)
	{
		cc = &reg->code[i];
		switch (cc->type)
		{
			case re_string:
				printf("cc: string\n");
				printf("str: '%s'(%d)\n", cc->str, cc->str_len);
				break;
			case re_set:
				printf("cc: set\n");
				printf("set: 0x%016lX%016lX\n", cc->set[1], cc->set[0]);
				break;
			default:
				printf("cc: undefined\n");
		}
		printf("repeat: min: %ld, max: %ld\n", cc->repeat[0], cc->repeat[1]);
	}
}

int regex_compile(regex_t *reg, unsigned char *exp, int flags)
{
	uint64_t		dot_set[2] = {~(uint64_t)0, ~(uint64_t)0};

	if (flags & RF_NO_NEWLINE_DOT)
		set_insert(dot_set, '\n', 1);
	if (flags & RF_NO_NUL_DOT)
		set_insert(dot_set, '\0', 1);
	reg->code_size = 0;
	for (int exp_ind = 0; exp[exp_ind] != '\0'; ++exp_ind)
	{
		regex_code_t	*cc = &reg->code[reg->code_size];
		switch (exp[exp_ind])
		{
			case '[': //[1-9] [-+] [^-] [^0-9] [^z\W-q] (all but z or q)
				if (cc->type != re_undefined)
					cc = &reg->code[++reg->code_size];
				cc->type = re_set;
				int not = (exp[exp_ind + 1] == '^');
				if (not)
				{
					++exp_ind;
					cc->set[0] = cc->set[1] = ~(uint64_t)0;
				}
				else
					cc->set[0] = cc->set[1] = 0;
				cc->repeat[0] = cc->repeat[1] = 1;
				int r;
				if ((r = regex_set(&exp[++exp_ind], cc->set, not)) == -1)
					// error(); erange
					NULL;
				if (exp[exp_ind += r] != ']')
					// error(); non-terminated set
					NULL;
			break;
			case '*': case '+': case '?':
				// if (cc->type == re_undefined)
				// 	error(); dangling quantifier
				if (cc->type == re_string && cc->str_len > 1)
				{
					reg->code[++reg->code_size].type = re_set;
					reg->code[reg->code_size].set[0] = 0;
					reg->code[reg->code_size].set[1] = 0;
					set_insert(reg->code[reg->code_size].set, cc->str[--cc->str_len], 0);
					cc->str[cc->str_len] = '\0';
					cc = &reg->code[reg->code_size];
				}
				cc->repeat[0] = exp[exp_ind] == '+';
				cc->repeat[1] = -1 + 2 * (exp[exp_ind] == '?');
				++reg->code_size;
			break ;
			case '{':
				// if (cc->type == re_undefined)
				// 	error(); dangling quantifier
				if (cc->type == re_string && cc->str_len > 1)
				{
					reg->code[++reg->code_size].type = re_set;
					reg->code[reg->code_size].set[0] = 0;
					reg->code[reg->code_size].set[1] = 0;
					set_insert(reg->code[reg->code_size].set, cc->str[--cc->str_len], 0);
					cc->str[cc->str_len] = '\0';
					cc = &reg->code[reg->code_size];
				}
				++exp_ind;
				unsigned char *tmp;
				cc->repeat[0] = strtol(&exp[exp_ind], (char**)&tmp, 0);
				switch (*tmp)
				{
					case ',':
						if (*(++tmp) == '}')
						{
							cc->repeat[1] = -1;
							exp_ind += (unsigned long)tmp - (unsigned long)&exp[exp_ind];
						}
						else
						{
							cc->repeat[1] = strtol(tmp, (char**)&tmp, 0);
							// if (*tmp != '}')
							// 	error(); invalid character after y in {x,y}
							// else
							exp_ind += (unsigned long)tmp - (unsigned long)&exp[exp_ind];
							if (cc->repeat[0] > cc->repeat[1])
							{
								long _ = cc->repeat[0];
								cc->repeat[0] = cc->repeat[1];
								cc->repeat[1] = _;
							}
						}
					break;
					case '}':
						// if (tmp == &exp[exp_ind])
						// 	error(); empty {}
						cc->repeat[1] = cc->repeat[0];
					break;
					// default:
					// 	error(); invalid character after , in {}
				}
			break;
			case '.':
				if (cc->type != re_undefined)
					cc = &reg->code[++reg->code_size];
				cc->type = re_set;
				cc->set[0] = dot_set[0];
				cc->set[1] = dot_set[1];
				cc->repeat[0] = 1;
				cc->repeat[1] = 1;
			break;
			case '\\':
				printf("in backslash\n");
			break;
			default:
				if (cc->type != re_undefined)
					cc = &reg->code[++reg->code_size];
				cc->type = re_string;
				cc->repeat[0] = 1;
				cc->repeat[1] = 1;
				for (int pass = 0; pass < 2; ++pass)
				{
					cc->str_len = 0;
					int i = exp_ind;
					for (; exp[i] != '\0'; ++i)
					{
						unsigned char *tmp;
						if ((tmp = strchr(REGEX_META_CHAR, exp[i])) != NULL)
							break;
						else if (exp[i] == '\\')
						{
							char c;
							int index;
							c = unescape(&exp[i], 0, &index);
							i += index - 1;
							if (c >= 0)
								if (!pass)
									++cc->str_len;
								else
									cc->str[cc->str_len++] = c;
							else
								break;
						}
						else
							if (!pass)
								++cc->str_len;
							else
								cc->str[cc->str_len++] = exp[i];
					}
					if (pass)
						exp_ind = i - 1;
					if (!pass && (cc->str = malloc(cc->str_len + 1)) == NULL)
					// 	error();
						NULL;
					cc->str[cc->str_len] = '\0';
				}
			break;
		}
	}
	if (reg->code[reg->code_size].type != re_undefined)
		++reg->code_size;
}

/*
typedef struct	s_regex_cursor
{
	unsigned long	index;
	unsigned long	line;
	unsigned long	column;
	unsigned long	vtab_count;
}				t_regex_cursor;

typedef struct	s_regex_found
{
	t_regex_cursor	start;
	t_regex_cursor	end;
}				t_regex_found;
*/

t_regex_cursor	cursor_increment(unsigned char *str, t_regex_cursor cur)
{
	switch (str[cur.index++])
	{
		case '\n':
			++cur.line;
			cur.column = cur.vtab_count = 0;
			return (cur);
		case '\t':
			++cur.vtab_count;
		default:
			++cur.column;
	}
	return (cur);
}

// technically: a|b|c == a|(?:b|c)

static inline int	in_set(unsigned char c, uint64_t set[2])
{
	if (c > 127)
		return (0);
	if (c > 63)
		return (!!(((uint64_t)1 << (c - 64)) & set[1]));
	return (!!(((uint64_t)1 << c) & set[0]));
}

int	regex_find(regex_t *reg, unsigned char *str, t_regex_cursor cur, t_regex_found *found)
{
	t_regex_cursor	ccur = cur;
	int				code_ind;

	if (reg == NULL || str == NULL || found == NULL)
		return (0);

	for (code_ind = 0; code_ind < reg->code_size; ++code_ind)
	{
		if (str[cur.index] == '\0')
			return (0);
		regex_code_t	*cc = &reg->code[code_ind];
		int				match = 0;
		int				i;

		switch (cc->type)
		{
			case re_set:
				for (i = 0; i < cc->repeat[1]; ++i, ccur = cursor_increment(str, ccur))
					if (!in_set(str[ccur.index], cc->set))
						break;
				if (i < cc->repeat[0])
				{
					ccur = cur = cursor_increment(str, cur);
					code_ind = -1;
				}
				break;
			case re_string:
				for (i = 0; i < cc->str_len; ++i, ccur = cursor_increment(str, ccur))
					if (cc->str[i] != str[ccur.index])
						break;
				if (i != cc->str_len)
				{
					ccur = cur = cursor_increment(str, cur);
					code_ind = -1;
				}
		}
	}
	if (code_ind != reg->code_size)
		return (0);
	*found = (t_regex_found){.start = cur, .end = ccur};
	return (1);
}

void	regex_debug_print_found(char *str, t_regex_found found, int tab_length)
{
	printf("'%.*s'\n", (int)(found.end.index - found.start.index), &str[found.start.index]);
	printf("start: i: %lu\nline: %lu\ncolumn: %lu (%lu)\n", found.start.index,
		found.start.line, found.start.column,
		found.start.column + (tab_length - 1) * found.start.vtab_count);
	printf("end: i: %lu\nline: %lu\ncolumn: %lu (%lu)\n", found.end.index,
		found.end.line, found.end.column,
		found.end.column + (tab_length - 1) * found.end.vtab_count);
}

int	main(int argc, char **argv)
{
	regex_t			reg = {{0}, 0};
	t_regex_cursor	start = {0, 0, 0, 0};
	t_regex_found	found;

	if (argc < 3)
	{
		printf("expected: <regex> <string> [...]\n");
		exit(0);
	}
	regex_compile(&reg, argv[1], RF_NO_NUL_DOT | RF_NO_NEWLINE_DOT);
	// regex_debug_print_code(&reg);
	if (regex_find(&reg, argv[2], start, &found))
	{
		regex_debug_print_found(argv[2], found, 8);
		while (regex_find(&reg, argv[2], cursor_increment(argv[2], found.start), &found))
			regex_debug_print_found(argv[2], found, 8);
	}
	else
		printf("no match\n");
}
