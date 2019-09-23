#ifndef __SDL_HEAD_H
#define __SDL_HEAD_H

#include "libft.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#define OK 1
#define ERROR 0

/*
** timer status
*/

# define TIMER_OFF				0
# define TIMER_ON				1
# define TIMER_PAUSED			2

typedef struct					s_timer
{
    Uint32						when_started;
    Uint32						when_paused;
    int							status;
}								t_timer;

typedef struct		s_point
{
	int				x;
	int				y;
}					t_point;

typedef	struct		s_sdl
{
	t_point			win_size;
	char			*title;
	SDL_Window		*win;
	SDL_Renderer	*ren;
	SDL_Surface		*surf;
	float			fps;
	unsigned        frame_id;
}					t_sdl;

typedef struct		s_pr
{
	SDL_Window		*win;
	SDL_Renderer	*rend;

	SDL_Surface		*texture;

	SDL_Surface		*background;
	SDL_Surface		*play_button;
	SDL_Surface		*exit_button;
	SDL_Surface		*logo;
	SDL_Surface		*choose_level_button;
	SDL_Surface		*font_texture;

	SDL_Rect		play_rect;
	SDL_Rect		exit_rect;
	SDL_Rect		logo_rect;
	SDL_Rect		choose_level_rect;
	SDL_Rect		font_rect;

	SDL_Color		font_color;

	SDL_bool		done;

	TTF_Font		*font;

	char			**maps;

	int				sw;

	SDL_Event		event;

	int				i;
	int				maxi;

	int				win_h;
	int				win_w;
}					t_pr;

//MENU
void				initialize_sdl_win(t_pr *m);
void				load_textures(t_pr *m, t_sdl *sdl);
void				readdirec(t_pr *m, t_sdl *sdl, char **maps, int max_maps);
void				set_text(t_pr *m, char *text, t_sdl *sdl);
void				renderallshit(t_pr *m);
void				down_action(t_pr *m);
void				up_action(t_pr *m);
void				rest_of_the_action_shit(t_pr *m, Uint8 *menu, t_sdl *sdl);
SDL_Rect			change_size(SDL_Rect rect);
SDL_Rect			reset_size(SDL_Rect rect);


//					CREATE
t_sdl				*new_t_sdl(int win_size_x, int win_size_y, const char *title);
int					init_sdl(t_sdl *sdl);

//					INFO
int					error_message(const char *error);
void				*error_message_null(const char *error);

//					IMAGES
SDL_Surface			*load_jpg_png(const char *file_name);
SDL_Surface			*load_bmp(const char *file_name);
SDL_Surface			*load_optimize_bmp(const char *file_name);
SDL_Surface			*load_img(const char *file_name);

//					TEXTURE
SDL_Texture			*load_texture(const char *file_name, SDL_Renderer *ren);
SDL_Texture			*texture_from_surf(SDL_Surface *surf, SDL_Renderer *ren);
SDL_Texture			*new_fresh_texture(SDL_Renderer *ren, int width, int height);

//					RENDER
void				sdl_render(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect *src, SDL_Rect *dst);

//					FONT AND TEXT

SDL_Texture			*make_color_text(SDL_Renderer *ren, const char *str, const char *file_font, int size, SDL_Color col);
SDL_Texture			*make_black_text(SDL_Renderer *ren, const char *str, const char *path_to_font, int size);
SDL_Texture			*make_text_using_ttf_font(SDL_Renderer *ren, TTF_Font *font, const char *str, SDL_Color col);
SDL_Texture			*make_black_text_using_ttf_font(SDL_Renderer *ren, TTF_Font *font, const char *str);

//					DESTROY
void				quit_sdl();
void				free_t_sdl(t_sdl **s);
void				close_t_sdl(t_sdl *s);

//					TIMER
Uint32							get_ticks(t_timer timer);
void							stop_timer(t_timer *timer);
void							unpause_timer(t_timer *timer);
void							pause_timer(t_timer *timer);
void							start_timer(t_timer *timer);
t_timer							init_timer(void);

//					PIXELS
void				put_pixel(SDL_Surface *dst, int x, int y, Uint32 pixel);
Uint32				get_pixel(SDL_Surface *src, int x, int y);

#endif
