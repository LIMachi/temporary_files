/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_set.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:34:57 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 13:35:17 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

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
