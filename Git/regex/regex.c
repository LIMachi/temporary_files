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
{"a", {1, 1}, "bcdz", {0, -1}, "\n", {1, 1}}

typedef enum	regex_type_e
{
	re_undefined = 0,
	re_string,
	re_set,
	re_nset,
	re_or
}				regex_type_t;

#include <stdint.h>

typedef struct	regex_code_s
{
	regex_type_t		type;
	union {
		struct {
			char			*str;
			int				str_len;
		};
		uint64_t		set[2]; //flags to all 128 ascii characters
		int				choices[2]; //indexes
		struct {
			int			group_index;
			int			group_flags;
		};
	};
	long				repeat[2];
}				regex_code_t;

#define SET_SET_CODE(set, x) if ((x) > 63) (set)[1] |= ((uint64_t)1) << ((x) - 64); else (set)[0] |= ((uint64_t)1) << (x);
#define UNSET_SET_CODE(set, x) if ((x) > 63) (set)[1] &= ~(((uint64_t)1) << ((x) - 64)); else (set)[0] &= ~(((uint64_t)1) << (x));

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
#define REGEX_META_CHAR ".[{()\\*+?|^$"
//                       ooo  ooooo
#define REGEX_ESCAPE_GROUP "wWsSdD" //word, space, digit, maj = ^
									//[A-Za-z0-9_], [\t\r\n\f ], [0-9]
#define REGEX_ESCAPE_META "dlsuwDLSUWbBAzZ123456789"

typedef enum	regex_flags_e
{
	RF_NONE = 0,
	RF_NO_NEWLINE_DOT = 1,
	RF_NO_NUL_DOT = 2
}				regex_flag_t;

int regex_compile(regex_t *reg, char *exp, int flags)
{
	uint64_t		dot_set[2] = {~(uint64_t)0, ~(uint64_t)0};
	const uint64_t	w_set[2] = {0x0, 0x0};
	const 

	if (flags & RF_NO_NEWLINE_DOT)
		UNSET_SET_CODE(dot_set, '\n');
	if (flags & RF_NO_NUL_DOT)
		UNSET_SET_CODE(dot_set, '\0');
	reg->code_size = 0;
	for (int exp_ind = 0; exp[exp_ind] != '\0'; ++exp_ind)
	{
		regex_code_t	*cc = &reg->code[reg->code_size];
		switch (exp[exp_ind])
		{
			case '[': //[1-9] [-+] [^-] [^0-9] [^z\W-q] (all but z or q)
				if (cc->type != re_undefined)
					cc = &reg->code[++reg->code_size];
				// if (strchr(&exp[exp_ind], ']') == NULL)
				// 	error();
				if (exp[exp_ind + 1] == '^')
				{
					cc->type = re_nset;
					++exp_ind;
				}
				else
					cc->type = re_set;
				cc->repeat[0] = 1;
				cc->repeat[1] = 1;
				cc->set[0] = 0;
				cc->set[1] = 0;
				for (int i = ++exp_ind; exp[i] != ']'; ++i)
				{
					if (exp[i + 1] == '-')
					{

					}
					else if (exp[i] == '\\')
					{
						strchr()
					}
					else
						SET_SET_CODE(cc->set, exp[i]);
				}
			break;
			case '*': case '+': case '?':
				// if (cc->type == re_undefined)
				// 	error();
				if (cc->type == re_string && cc->str_len > 1)
				{
					reg->code[++reg->code_size].type == re_set;
					reg->code[reg->code_size].set[0] = 0;
					reg->code[reg->code_size].set[1] = 0;
					SET_SET_CODE(reg->code[reg->code_size].set, cc->str[--cc->str_len - 1]);
					cc->str[cc->str_len - 1] = '\0';
					cc = &reg->code[reg->code_size];
				}
				cc->repeat[0] = exp[exp_ind] == '+';
				cc->repeat[1] = -1 + 2 * (exp[exp_ind] == '?');
				++reg->code_size;
			break ;
			case '{':
				// if (cc->type == re_undefined)
				// 	error();
				if (cc->type == re_string && cc->str_len > 1)
				{
					reg->code[++reg->code_size].type == re_set;
					reg->code[reg->code_size].set[0] = 0;
					reg->code[reg->code_size].set[1] = 0;
					SET_SET_CODE(reg->code[reg->code_size].set, cc->str[--cc->str_len - 1]);
					cc->str[cc->str_len - 1] = '\0';
					cc = &reg->code[reg->code_size];
				}
				++exp_ind;
				// if (strchr(&exp[exp_ind], '}') == NULL)
				// 	error();
				char *tmp;
				cc->repeat[0] = strtol(&exp[exp_ind], &tmp, 0);
				switch (*tmp)
				{
					case ',':
						if (*(++tmp) == '}')
							cc->repeat[1] = -1;
						else
						{
							cc->repeat[1] = strtol(tmp, &tmp, 0);
							// if (*tmp != '}')
							// 	error();
							// else
							exp_ind += (size_t)tmp - (size_t)&exp[exp_ind];
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
						// 	error();
						cc->repeat[1] = cc->repeat[0];
					break;
					// default:
					// 	error();
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
					int i = exp_ind + 1;
					for (; exp[i] != '\0'; ++i)
					{
						char *tmp;
						if ((tmp = strchr(REGEX_META_CHAR, exp[i])) != NULL)
							if (*tmp == '\\' && strchr(REGEX_ESCAPE_META, exp[i + 1]) == NULL)
							{
								++i;
								if (!pass)
									continue;
								else
									cc->string[cc->str_len++] = exp[i + 1];
							}
							else
								break;
						else
							if (!pass)
								continue;
							else
								cc->string[cc->str_len++] = exp[i];
					}
					if (pass)
						exp_ind = i - 1;
					if (!pass && (cc->str = malloc(cc->str_len)) == NULL)
					// 	error();
						NULL;
					cc->str[cc->str_len - 1] = '\0';
				}
			break;
		}
	}
}

/*
int regex_compile(regex_t *reg, char *exp, int flags)
{
	if (exp == NULL || reg == NULL)
		return (-1);
	for (int i = 0; exp[i] != '\0'; ++i)
	{
		switch (exp[i])
		{
			case '.':
				//any char, might or not match \n
			break;
			case '[':
				//set
				//unset
				//classe
			break;
			case '^':
				//start of line/buffer
			break;
			case '$':
				//end of line/buffer
			break;
			case '(':
				//group
			break;
			case '?':
			case '+':
			case '*':
			case '{':
				//quantifiers
			break;
			case '|':
				//either (or)
			break;
			case '\\':
				//followed by number: reference to group
				//followed by meta-character: set
				//followed by escapable: escaped
			break;
		}
	}
}*/