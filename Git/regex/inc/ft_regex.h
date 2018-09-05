/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_regex.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:33:26 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 13:34:41 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_REGEX_H
# define FT_REGEX_H

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

#endif
