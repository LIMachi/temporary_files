/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_debug_print_found.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/05 13:38:14 by hmartzol          #+#    #+#             */
/*   Updated: 2018/09/05 13:38:27 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_regex.h>

void	regex_debug_print_found(char *str, t_regex_found found, int tab_length)
{
	printf("'%.*s'\n", (int)(found.end.index - found.start.index), &str[found.start.index]);
	printf("start: i: %lu\nline: %lu\ncolumn: %lu (%lu)\n", found.start.index,
		found.start.line, found.start.column,
		found.start.column + (tab_length - 1) * found.start.vtab_count);
	printf("end: i: %lu\nline: %lu\ncolumn: %lu (%lu)\n", found.end.index,
		found.end.line, found.end.column,
		found.end.column + (tab_length - 1) * found.end.vtab_count);
}
