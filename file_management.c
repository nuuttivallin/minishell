/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nvallin <nvallin@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 11:38:47 by nvallin           #+#    #+#             */
/*   Updated: 2024/09/26 11:53:37 by nvallin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_check_in_files(t_command *cmd)
{
	t_file	*current;
	int		fd;

	current = cmd->redir_in_start;
	while (current)
	{
		if (current->type != HEREDOC)
		{
			fd = open(current->name, O_RDONLY);
			if (fd < 0)
			{
				perror("minishell");
				return (1);
			}
			close (fd);
		}
		if (current == cmd->redir_in_end)
			current = NULL;
		else
			current = current->next;
	}
	return (0);
}

int	ft_check_out_files(t_command *cmd)
{
	t_file	*current;
	int		fd;

	current = cmd->redir_out_start;
	while (current)
	{
		if (current->type == ADD_TO)
			fd = open(current->name, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (current->type == CREATE)
			fd = open(current->name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			perror("minishell");
			return (1);
		}
		else
			close (fd);
		if (current == cmd->redir_out_end)
			current = NULL;
		else
			current = current->next;
	}
	return (0);
}

int	ft_check_files_helper(t_file *current, int last)
{
	int	fd;

	while (current && current->index <= last)
	{
		if (current->type == ADD_TO)
			fd = open(current->name, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (current->type == CREATE)
			fd = open(current->name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (current->type == OPEN)
			fd = open(current->name, O_RDONLY);
		if (current->type != HEREDOC && fd < 0)
		{
			perror("minishell");
			return (1);
		}
		else if (current->type != HEREDOC)
			close (fd);
		current = current->next;
	}
	return (0);
}

int	ft_check_files(t_command *cmd)
{
	int		last;
	t_file	*current;

	last = 0;
	current = NULL;
	if (cmd)
	{
		if (cmd->redir_in_start)
		{
			current = cmd->redir_in_start;
			last = cmd->redir_in_end->index;
		}
		if (cmd->redir_out_start)
		{
			if (current == NULL || current->index > cmd->redir_out_start->index)
				current = cmd->redir_out_start;
			if (last < cmd->redir_out_end->index)
				last = cmd->redir_out_end->index;
		}
		if (current != NULL && ft_check_files_helper(current, last))
			return (1);
	}
	return (0);
}

void	ft_add_file_to_list(t_command_table *table, t_file *temp)
{
	t_file	*file;
	int		i;

	i = 0;
	file = NULL;
	if (table->files == NULL)
	{
		table->files = temp;
		table->files->index = 0;
	}
	else
	{
		file = table->files;
		while (file->next != NULL)
		{
			file = file->next;
			file->index = ++i;
		}
		file->next = temp;
		file->next->index = ++i;
		temp->prev = file;
	}
}
