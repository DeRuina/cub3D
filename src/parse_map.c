/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tspoof <tspoof@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 14:31:07 by tspoof            #+#    #+#             */
/*   Updated: 2023/11/08 13:13:13 by tspoof           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"


int valid_colors(t_map *map)
{
	if (map->cealing_color == -1 || map->floor_color == -1)
		return (0);
	return (1);
}

int valid_textures(t_map *map)
{
	if (!map->textures.no || !map->textures.we || !map->textures.ea
		 || !map->textures.so)
		 	return (0);
	return (1);
}


int is_edge(int j, int x_max, int i, int y_max)
{
	if (i <= 0 || i >= y_max)
		return (1);
	if (j <= 0 || j >= x_max)
		return (1);
	return (0);
}

int is_player(char c)
{
	static char player[] = {'N', 'W', 'E', 'S', '\0'};
	int	i;

	i = 0;
	while(player[i])
	{
		if (player[i] == c)
			return (1);
		i++;
	}
	return (0);
}
static int check_row(char **rows, int i, int j, int len)
{
	if (rows[i][j] == '0' || is_player(rows[i][j]))
	{
		if (is_edge(j, ft_strlen(rows[i]), i, len))
			return (0);
		if (rows[i - 1][j] != '1' && rows[i - 1][j] != '0'
			&& !is_player(rows[i - 1][j]))
			return (0);
		if (rows[i + 1][j] != '1' && rows[i + 1][j] != '0'
			&& !is_player(rows[i + 1][j]))
			return (0);
		if (rows[i][j + 1] != '1' && rows[i][j + 1] != '0'
			&& !is_player(rows[i][j + 1]))
			return (0);
		if (rows[i][j - 1] != '1' && rows[i][j - 1] != '0'
			&& !is_player(rows[i][j - 1]))
			return (0);
	}
	return (1);
}

static void count_player(int *player_count)
{
	(*player_count)++;
	if (*player_count > 1)
		dt_error(INVALID_MAP);
}

int valid_map(t_map *map)
{
	size_t i;
	size_t j;
	char **rows;
	int player_count;

	player_count = 0;
	i = 0;
	rows = (char **)map->map->memory;
	while (map->map->len != 0 && i < map->map->len)
	{
		j = 0;
		while (rows[i] && rows[i][j])
		{
			if (is_player(rows[i][j]))
				count_player(&player_count);
			if (!check_row(rows, i, j, map->map->len))
				dt_error(INVALID_MAP);
			j++;
		}
		i++;
	}
	if (!player_count)
		dt_error(NO_PLAYER);
	return (1);
}


static void get_params(int fd, t_map *map)
{
	char	*line;
	char	*tmp;

	line = NULL;
	while ((line = get_next_line(fd)))
	{
		tmp = ft_strtrim(line, " \t");
		if (!tmp)
			dt_error(MALLOC_FAIL);
		free(line);
		line = tmp;
		(void)store_map_params(&line, map);
		free(line);
		line = NULL;
		if (valid_colors(map) && valid_textures(map))
			return;
	}
}

static void get_map(int fd, t_map *map)
{
	char	*line;

	line = NULL;
	while ((line = get_next_line(fd)))
	{
		store_map(line, map);
		free(line);
		line = NULL;
	}
}

static void check_validity(t_map *map)
{
	if (!map->map->len || !valid_map(map))
		dt_error(INVALID_MAP);
	if (!valid_colors(map))
		dt_error(INVALID_COLOR);
	if (!valid_textures(map))
		dt_error(INVALID_TEXTURE);
}

int	parse_map(char *path, t_map *map)
{
	int		fd;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		dt_error(FILE_OPEN);
	get_params(fd, map);
	get_map(fd, map);
	check_validity(map);
	close(fd);
	return (0);
}
