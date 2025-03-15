/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nvallin <nvallin@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 01:34:25 by nvallin           #+#    #+#             */
/*   Updated: 2024/09/19 01:34:25 by nvallin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_replace_env_value(char ***envp, char *new)
{
	int	i;

	i = 0;
	while (envp[0][i] && ft_envcmp(envp[0][i], new))
		i++;
	if (envp[0][i])
	{
		free(envp[0][i]);
		envp[0][i] = ft_strdup(new);
		if (!envp[0][i])
		{
			while (envp[0][++i])
				free(envp[0][i]);
			ft_free_array(envp[0]);
			write(2, "malloc failed while replacing env value\n", 40);
			return (1);
		}
	}
	else
	{
		if (!ft_array_append(&envp[0], new))
			return (1);
	}
	return (0);
}

int	ft_find_env_start(char *str, int i)
{
	while (str[i] != '\0' && str[i] != '$')
		i++;
	return (i);
}

int	ft_find_env_end(char *str, int start)
{
	int	end;

	end = start + 1;
	if (str[end] != '_' && !ft_isalpha(str[end]))
	{
		if (str[end] != '\0' && !ft_is_whitespace(str[end]))
			return (end + 1);
		return (end);
	}
	while (str[end] && (ft_isalnum(str[end]) || str[end] == '_'))
		end++;
	return (end);
}

int	ft_get_path(char ***path_arr, t_command *cmd)
{
	char	*path;
	int		i;

	i = 0;
	path = ft_getenv("PATH", *cmd->envp);
	if (path == NULL || path[0] == '\0')
	{
		if (!ft_array_append(&path_arr[0], "/"))
			return (0);
		return (1);
	}
	path_arr[0] = ft_split(path, ':');
	if (path_arr[0] == NULL)
		return (0);
	while (path_arr[0][i])
	{
		if (!ft_strcombine(&path_arr[0][i], "/"))
		{
			write(2, "ft_strcombine failed in ft_get_path\n", 36);
			ft_free_array(path_arr[0]);
			return (0);
		}
		i++;
	}
	return (1);
}
