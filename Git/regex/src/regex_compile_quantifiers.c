/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_compile_quantifiers.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 14:00:21 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 14:05:16 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>


/*
** -1: dangling quantifier
*/

int	regex_compile_basic_quantifiers(regex_t *reg, unsigned char *exp, regex_code_t **cc)
{
	if ((*cc)->type == re_undefined)
		return (-1);
	if ((*cc)->type == re_string && (*cc)->str_len > 1)
	{
		reg->code[++reg->code_size].type = re_set;
		reg->code[reg->code_size].set[0] = 0;
		reg->code[reg->code_size].set[1] = 0;
		set_insert(reg->code[reg->code_size].set, (*cc)->str[--(*cc)->str_len], 0);
		(*cc)->str[(*cc)->str_len] = '\0';
		(*cc) = &reg->code[reg->code_size];
	}
	(*cc)->repeat[0] = exp[exp_ind] == '+';
	(*cc)->repeat[1] = -1 + 2 * (exp[exp_ind] == '?');
	++reg->code_size;
}

/*
** -1: dangling quantifier
** -2: invalid character after y in {x,y}
** -3: empty {}
** -4: invalid character after , in {}
*/

static inline	sif_t(regex_t *reg, unsigned char *exp, regex_code_t **cc,
					char *tmp)
{
	long	swap;

	if (*(++tmp) == '}')
	{
		(*cc)->repeat[1] = -1;
		exp_ind += (unsigned long)tmp - (unsigned long)&exp[exp_ind];
	}
	else
	{
		(*cc)->repeat[1] = strtol(tmp, (char**)&tmp, 0);
		if (*tmp != '}')
			return (-2);
		exp_ind += (unsigned long)tmp - (unsigned long)&exp[exp_ind];
		if ((*cc)->repeat[0] > (*cc)->repeat[1])
		{
			swap = (*cc)->repeat[0];
			(*cc)->repeat[0] = (*cc)->repeat[1];
			(*cc)->repeat[1] = swap;
		}
	}
	return (0);
}

int	regex_compile_extended_quantifier(regex_t *reg, unsigned char *exp,
									regex_code_t **cc)
{
	unsigned char *tmp;

	if ((*cc)->type == re_undefined)
		return (-1);
	if ((*cc)->type == re_string && (*cc)->str_len > 1)
	{
		reg->code[++reg->code_size].type = re_set;
		reg->code[reg->code_size].set[0] = 0;
		reg->code[reg->code_size].set[1] = 0;
		set_insert(reg->code[reg->code_size].set, (*cc)->str[--(*cc)->str_len], 0);
		(*cc)->str[(*cc)->str_len] = '\0';
		(*cc) = &reg->code[reg->code_size];
	}
	(*cc)->repeat[0] = strtol(&exp[++exp_ind], (char**)&tmp, 0);
	if (*tmp == ',')
		return (sif_t(reg, exp, cc, tmp));
	else if (*tmp == '}')
	{
		if (tmp == &exp[exp_ind])
			return (-3);
		(*cc)->repeat[1] = (*cc)->repeat[0];
	}
	else
		return (-4);
	return (0);
}
