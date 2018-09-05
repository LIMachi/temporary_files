/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unescape.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:32:44 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 13:33:16 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

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
