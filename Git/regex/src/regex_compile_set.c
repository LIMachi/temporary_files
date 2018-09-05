/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_compile_set.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 14:06:45 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 14:07:03 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

/*
** -1: erange
** -2: non-terminated set
*/

int	regex_compile_set(regex_t *reg, unsigned char *exp, regex_code_t **cc)
{
	int	not;

	if ((*cc)->type != re_undefined)
		*cc = &reg->code[++reg->code_size];
	(*cc)->type = re_set;
	not = (exp[exp_ind + 1] == '^');
	if (not)
	{
		++exp_ind;
		(*cc)->set[0] = (*cc)->set[1] = ~(uint64_t)0;
	}
	else
		(*cc)->set[0] = (*cc)->set[1] = 0;
	(*cc)->repeat[0] = (*cc)->repeat[1] = 1;
	int r;
	if ((r = regex_set(&exp[++exp_ind], (*cc)->set, not)) == -1)
		return (-1);
	if (exp[exp_ind += r] != ']')
		return (-2);
	return (0);
}
