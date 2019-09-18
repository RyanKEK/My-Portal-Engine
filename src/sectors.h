#ifndef __SECTORS_H
#define __SECTORS_H
#include "sector_worker.h"

typedef struct 		s_read_holder
{
	SDL_Surface		**textures;
	t_wall			**walls;
	t_item			*all_items;
	char			*maps_path[5];
	int				text_count;
	int				vect_count;
	int				wall_count;
}					t_read_holder;

int 				read_game_config_file(t_read_holder *holder, char *info_file_path);
t_sector			*read_map(char *pth, t_read_holder *holder);
int					get_num_from_str(char *str);
char				*skip_row_number(char *line);
void				get_count_struct_arrays(int fd, int *vect_count, int *wall_count);
unsigned			get_numbers(float *one, float *two, char delimiter, char *line);

enum item_type		get_item_type(char *line);
void 				load_animation(t_item *item, char *item_name);

t_vector			*get_vectors(int fd, int vec_size);
t_wall				**get_walls(int fd, int wall_size, t_vector *vectors, SDL_Surface **textures);
t_sector			*make_sectors_list(int fd, t_wall **walls, SDL_Surface **textures, t_item *all_items);
t_item				*make_items(char *data, t_item *all_items);

#endif