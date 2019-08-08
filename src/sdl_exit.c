#include "sdl_head.h"

int					error_message(const char *error)
{
	ft_putstr("\x1b[31mError: \x1b[33m");
	ft_putendl(error);
	ft_putstr("\x1b[0m");
	return (ERROR);
}

void				*error_message_null(const char *error)
{
	error_message(error);
	return (NULL);
}

void				quit_sdl()
{
	if (TTF_WasInit())
		TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void				close_t_sdl(t_sdl *s)
{
	if (s->ren)
		SDL_DestroyRenderer(s->ren);
	s->ren = NULL;
	if (s->surf)
		SDL_FreeSurface(s->surf);
	s->surf = NULL;
	if (s->win)
		SDL_DestroyWindow(s->win);
	s->win = NULL;
	if(SDL_WasInit(SDL_INIT_AUDIO))
		SDL_AudioQuit();
}

void				free_t_sdl(t_sdl **s)
{
	close_t_sdl(*s);
	if ((*s)->title)
		ft_strdel(&(*s)->title);
	ft_memdel((void**)s);
}
