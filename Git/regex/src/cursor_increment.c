/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cursor_increment.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:36:51 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 13:41:06 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

t_regex_cursor	cursor_increment(unsigned char *str, t_regex_cursor cur)
{
	if (str[cur.index] == '\n')
	{
		++cur.line;
		cur.column = cur.vtab_count = 0;
		++cur.index;
		return (cur);
	}
	if (str[cur.index] == '\t')
		++cur.vtab_count;
	++cur.column;
	++cur.index;
	return (cur);
}
