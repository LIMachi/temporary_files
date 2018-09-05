/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_debug_print_code.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:35:49 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 13:36:03 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

void	regex_debug_print_code(regex_t *reg)
{
	regex_code_t	*cc;

	if (reg == NULL)
		return ;
	printf("code size: %lu\n", reg->code_size);
	for (int i = 0; i < reg->code_size; ++i)
	{
		cc = &reg->code[i];
		switch (cc->type)
		{
			case re_string:
				printf("cc: string\n");
				printf("str: '%s'(%d)\n", cc->str, cc->str_len);
				break;
			case re_set:
				printf("cc: set\n");
				printf("set: 0x%016lX%016lX\n", cc->set[1], cc->set[0]);
				break;
			default:
				printf("cc: undefined\n");
		}
		printf("repeat: min: %ld, max: %ld\n", cc->repeat[0], cc->repeat[1]);
	}
}
