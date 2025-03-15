/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nvallin <nvallin@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 11:34:11 by nvallin           #+#    #+#             */
/*   Updated: 2024/09/18 19:52:53 by nvallin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_export(t_command *cmd, t_command_table *table)
{
	int	i;

	i = 1;
	table->exit_status = 1;
	if (cmd->argc == 1)
		ft_env(*cmd->envp, 1);
	else
	{
		while (cmd->argv[i])
		{
			if (!ft_export_syntax_check(cmd->argv[i]))
				return (1);
			if (ft_replace_env_value(&*cmd->envp, cmd->argv[i]))
				return (1);
			i++;
		}
	}
	table->exit_status = 0;
	return (0);
}

int	ft_unset(t_command *cmd, t_command_table *table)
{
	char	**dup;
	int		len;
	int		i;

	i = -1;
	table->exit_status = 1;
	if (cmd->argv[1])
	{
		len = ft_strlen(cmd->argv[1]);
		while (cmd->envp[0][++i])
		{
			if (len == ft_envlen(cmd->envp[0][i]) && \
			!ft_strncmp(cmd->envp[0][i], cmd->argv[1], len))
			{
				dup = malloc(sizeof (char *) * ft_arrlen(*cmd->envp));
				if (dup)
					dup = ft_array_cpynfree(dup, &**cmd->envp, cmd->argv[1]);
				if (!dup)
					return (1);
				*cmd->envp = dup;
			}
		}
	}
	table->exit_status = 0;
	return (0);
}

int	ft_pwd(t_command_table *table)
{
	char	*cwd;

	cwd = NULL;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: pwd");
		table->exit_status = 1;
		return (1);
	}
	printf("%s\n", cwd);
	free(cwd);
	table->exit_status = 0;
	return (0);
}

int	ft_cd(t_command *cmd, t_command_table *table)
{
	char	*oldpwd;

	if (cmd->argc < 2)
		return (0);
	table->exit_status = 1;
	if (cmd->argc == 2)
	{
		oldpwd = getcwd(NULL, 0);
		if (!oldpwd)
			write(2, "minishell: cd: getcwd failed\n", 29);
		else if (!ft_strprepend(&oldpwd, "OLDPWD="))
			write(2, "minishell: cd: ft_strprepend failed\n", 36);
		else if (chdir(cmd->argv[1]))
			perror("minishell: cd");
		else if (!ft_update_pwd(cmd, oldpwd))
			table->exit_status = 0;
		if (oldpwd)
			free(oldpwd);
		if (table->exit_status == 0)
			return (0);
	}
	if (cmd->argc > 2)
		write(2, "minishell: cd: too many arguments\n", 34);
	return (1);
}

int	ft_builtin(t_command *cmd, t_command_table *table)
{
	int	result;

	result = 0;
	table->exit_status = 0;
	if (cmd->argv && !ft_strncmp(cmd->argv[0], "env", 4))
		ft_env(*cmd->envp, 0);
	if (cmd->argv && !ft_strncmp(cmd->argv[0], "export", 7))
		result = ft_export(cmd, table);
	if (cmd->argv && !ft_strncmp(cmd->argv[0], "unset", 6))
	{
		result = ft_unset(&*cmd, table);
		if (result)
			write(2, "minishell: unset: error\n", 24);
	}
	if (cmd->argv && !ft_strncmp(cmd->argv[0], "pwd", 4))
		result = ft_pwd(table);
	if (cmd->argv && !ft_strncmp(cmd->argv[0], "cd", 3))
		result = ft_cd(cmd, table);
	if (cmd->argv && !ft_strncmp(cmd->argv[0], "echo", 5))
		result = ft_echo(cmd);
	if (cmd->argv && !ft_strncmp(cmd->argv[0], "exit", 5))
		result = ft_exit(cmd, table);
	return (result);
}
