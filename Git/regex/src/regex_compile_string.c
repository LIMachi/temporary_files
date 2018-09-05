/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_compile_string.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 14:17:07 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 14:17:22 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

/*
** -1: allocation error
*/

int	regex_compile_string(regex_t *reg, unsigned char *exp, regex_code_t **cc)
{
	unsigned char	*tmp;
	int				i;
	char			c;
	int				index;
	int				pass;

	if ((*cc)->type != re_undefined)
		(*cc) = &reg->code[++reg->code_size];
	(*cc)->type = re_string;
	(*cc)->repeat[0] = 1;
	(*cc)->repeat[1] = 1;
	for (pass = 0; pass < 2; ++pass)
	{
		(*cc)->str_len = 0;
		for (i = exp_ind; exp[i] != '\0'; ++i)
		{
			if ((tmp = strchr(REGEX_META_CHAR, exp[i])) != NULL)
				break;
			else if (exp[i] == '\\')
			{
				c = unescape(&exp[i], 0, &index);
				i += index - 1;
				if (c >= 0)
					if (!pass)
						++(*cc)->str_len;
					else
						(*cc)->str[(*cc)->str_len++] = c;
				else
					break;
			}
			else
				if (!pass)
					++(*cc)->str_len;
				else
					(*cc)->str[(*cc)->str_len++] = exp[i];
		}
		if (pass)
			exp_ind = i - 1;
		if (!pass && ((*cc)->str = malloc((*cc)->str_len + 1)) == NULL)
			return (-1);
		(*cc)->str[(*cc)->str_len] = '\0';
	}
	return (0);
}
