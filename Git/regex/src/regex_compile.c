/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_compile.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:36:16 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 14:25:12 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

int	regex_compile(regex_t *reg, unsigned char *exp, int flags)
{
	uint64_t		dot_set[2] = {~(uint64_t)0, ~(uint64_t)0};
	int				r;
	int				exp_ind;

	if (flags & RF_NO_NEWLINE_DOT)
		set_insert(dot_set, '\n', 1);
	if (flags & RF_NO_NUL_DOT)
		set_insert(dot_set, '\0', 1);
	reg->code_size = 0;
	exp_ind = -1;
	while (exp[++exp_ind] != '\0')
	{
		regex_code_t	*cc = &reg->code[reg->code_size];
		if (exp[exp_ind] == '[')
			r = regex_compile_set(reg, exp, &cc);
		else if (exp[exp_ind] == '*' || exp[exp_ind] == '+' || exp[exp_ind] == '?')
			r = regex_compile_basic_quantifiers(reg, exp, &cc);
		else if (exp[exp_ind] == '{')
			r = regex_compile_extended_quantifier(reg, exp, &cc);
		else if (exp[exp_ind] == '.')
			r = regex_compile_dot(reg, exp, &cc);
		else if (exp[exp_ind] == '\\')
		{
			r = 0;
			printf("in backslash\n");
		}
		else
			r = regex_compile_string(reg, exp, &cc);
		if (r)
			return (r);
	}
	if (reg->code[reg->code_size].type != re_undefined)
		++reg->code_size;
	return (0);
}
