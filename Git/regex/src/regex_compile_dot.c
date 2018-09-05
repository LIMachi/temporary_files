/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_compile_dot.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 14:11:51 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 14:12:10 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

int	regex_compile_dot(regex_t *reg, unsigned char *exp, regex_code_t **cc)
{
	if ((*cc)->type != re_undefined)
		(*cc) = &reg->code[++reg->code_size];
	(*cc)->type = re_set;
	(*cc)->set[0] = dot_set[0];
	(*cc)->set[1] = dot_set[1];
	(*cc)->repeat[0] = 1;
	(*cc)->repeat[1] = 1;
	return (0);
}
