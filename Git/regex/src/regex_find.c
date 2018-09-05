/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_find.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:37:43 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 13:37:56 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

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
