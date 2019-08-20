#include "main_head.h"
#define EyeHeight  5
#define DuckHeight 2.5
#define HeadMargin 1
#define KneeHeight 2
//#define horfov (1.0 * 0.73f*H/W)
//#define verfov (1.0 * .2f)
#define Yaw(y,z) (y + z*player.yaw)

#define THREADS 4

float scaleH = 16;


static float		len_between_points(t_vector a, t_vector b)
{
	return (sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)));
}

float				get_triangle_height(t_vector a, t_vector b, t_vector c)
{
	int				ab;
	int				bc;
	int				ca;
	int				p;

	ab = len_between_points(a, b);
	bc = len_between_points(b, c);
	ca = len_between_points(c, a);
	p = (ab + bc + ca) >> 1;
	return (2 * (sqrt (p * (p - ab) * (p - bc) * (p - ca))) / ab);
}

static int		get_nearest_line_to_dot(t_vector a, t_vector d1, t_vector d2)
{
	int 		ad1;
	int			ad2;

	ad1 = len_between_points(a, d1);
	ad2 = len_between_points(a, d2);
	return min(ad1, ad2);
}

static void			sort_by_nearest(t_wall **walls, t_player player, signed short *tab, unsigned short max)
{
	int			i;
	int			j;
	float 		i_dist;
	float 		j_dist;
	int			tmp;
	
	i = 0;
	while (i < max && tab[i] != -1)
	{
		j = 0;
		i_dist = get_nearest_line_to_dot(player.pos, walls[tab[i]]->start, walls[tab[i]]->end);
		while (j < max && tab[j] != -1)
		{
			if (tab[i] != tab[j])
			{
				j_dist = get_nearest_line_to_dot(player.pos, walls[tab[j]]->start, walls[tab[j]]->end);
				if (i_dist < j_dist)
				{
					tmp = tab[i];
					tab[i] = tab[j];
					tab[j] = tmp;
				}
			}
			j++;
		}
		i++;
	}
}


static void 			make_intersect(t_wall *wall)
{
	t_vector 	i1;
	t_vector 	i2;
	float nearz = 1e-4f, farz = 5, nearside = 1e-5f, farside = 1000.f;

	i1 = Intersect(wall->start.x, wall->start.y, wall->end.x, wall->end.y, -nearside,nearz, -farside,farz);
    i2 = Intersect(wall->start.x, wall->start.y, wall->end.x, wall->end.y,  nearside,nearz,  farside,farz);
	if(wall->start.y < nearz) 
	{
		if(i1.y > 0)
		{
			wall->start.x = i1.x;
			wall->start.y = i1.y;
		}
		else
		{
			wall->start.x = i2.x;
			wall->start.y = i2.y;
		}
	}
    if(wall->end.y < nearz)
	{
		if(i1.y > 0)
		{
			wall->end.x = i1.x;
			wall->end.y = i1.y;
		}
		else
		{
			wall->end.x = i2.x;
			wall->end.y = i2.y;
		}
	}
}

void			draw_sectors(t_sector *sectors, t_player player, t_sdl *sdl, t_draw_data *data);

int t = 1;


static int			calc_floor_ceil(unsigned half_win_size_y, int floor_or_ceil_diff, float scale_y)
{
	return (half_win_size_y - floor_or_ceil_diff * scale_y);
}

static void			maping_wall_texture(int *u0, int *u1, float diff_start, float diff_end, float scaled_tex)
{
	*u0 = diff_start * scaled_tex;
	*u1 = diff_end * scaled_tex;
}
void 			draw_world(t_sector *sec, t_wall wall, t_player player, t_sdl *sdl, t_draw_data data)
{
	t_vector 	scale1;
	t_vector 	scale2;
	t_wall		cp;
	int 		ceil_y_s;
	int 		ceil_y_e;
	int 		floor_y_s;
	int 		floor_y_e;
	int 		n_ceil_y_s;
	int 		n_ceil_y_e;
	int 		n_floor_y_s;
	int 		n_floor_y_e;
	int 		cya;
	int 		cyb;
	int 		ya; 
	int 		yb;
	int 		n_cya;
	int 		n_cyb;
	int			x;
	int 		end;
	int 		u0, u1;
	float 		scaleL;
	int 		txtx;

	cp = wall;

	wall.start = (t_vector){wall.start.x - player.pos.x, wall.start.y - player.pos.y, wall.start.z - player.pos.z};
	wall.end = (t_vector){wall.end.x - player.pos.x, wall.end.y - player.pos.y, wall.end.z - player.pos.z};	

	wall.start = (t_vector){wall.start.x * player.sin_angl - wall.start.y * player.cos_angl,
				wall.start.x * player.cos_angl + wall.start.y * player.sin_angl, .z = wall.start.y};
	wall.end = (t_vector){wall.end.x * player.sin_angl - wall.end.y * player.cos_angl,
			wall.end.x * player.cos_angl + wall.end.y * player.sin_angl, .z = wall.end.y};
	
	if (wall.start.y <= 0 && wall.end.y <= 0)
		return ;

	t_vector org1 = {wall.start.x, wall.start.y}, org2 = {wall.end.x, wall.end.y};

	if (wall.start.y <= 0 || wall.end.y <= 0)
		make_intersect(&wall);
	if (wall.type != empty_wall)
	{
		if(fabsf(cp.start.x - cp.end.x) > fabsf(cp.start.y - cp.end.y))
    		scaleL = fabsf(cp.start.x - cp.end.x) / 10;
    	else
        	scaleL = fabsf(cp.start.y - cp.end.y) / 10;

		if(fabs(wall.end.x - wall.start.x) > fabs(wall.end.y - wall.start.y))
			maping_wall_texture(&u0, &u1, wall.start.x - org1.x, wall.end.x - org1.x, (wall.texture->w * scaleL - 1) / (org2.x - org1.x));
		else
			maping_wall_texture(&u0, &u1, wall.start.y - org1.y, wall.end.y - org1.y, (wall.texture->w * scaleL - 1) / (org2.y - org1.y));
	}

	scale1 =(t_vector) {player.hfov / wall.start.y, player.vfov / wall.start.y};
    scale2 =(t_vector) {player.hfov / wall.end.y, player.vfov / wall.end.y};

	wall.start.x = player.half_win_size.x - (int)(wall.start.x * scale1.x);
	wall.end.x = player.half_win_size.x - (int)(wall.end.x * scale2.x);

	if(wall.start.x >= wall.end.x || wall.end.x < data.start || wall.start.x > data.end)
		return ;
	ceil_y_s = calc_floor_ceil(player.half_win_size.y, data.diff_ceil, scale1.y);
	ceil_y_e = calc_floor_ceil(player.half_win_size.y, data.diff_ceil, scale2.y);
	floor_y_s = calc_floor_ceil(player.half_win_size.y, data.diff_floor, scale1.y);
	floor_y_e = calc_floor_ceil(player.half_win_size.y, data.diff_floor, scale2.y);
	
	if(wall.type == empty_wall)
	{
		n_ceil_y_s = calc_floor_ceil(player.half_win_size.y, min(wall.sectors[0]->ceil, wall.sectors[1]->ceil) - player.height, scale1.y);
		n_ceil_y_e = calc_floor_ceil(player.half_win_size.y, min(wall.sectors[0]->ceil, wall.sectors[1]->ceil) - player.height, scale2.y);
    	n_floor_y_s = calc_floor_ceil(player.half_win_size.y, max(wall.sectors[0]->floor, wall.sectors[1]->floor) - player.height, scale1.y);
		n_floor_y_e = calc_floor_ceil(player.half_win_size.y, max(wall.sectors[0]->floor, wall.sectors[1]->floor) - player.height, scale2.y);
	}

	x = max(wall.start.x, data.start);
	end = min(wall.end.x, data.end);

	data.start = x;
	data.end = end;
	while (x < end && x < sdl->win_size.x)
	{
		ya = (x - wall.start.x) * (ceil_y_e - ceil_y_s) / (wall.end.x - wall.start.x) + ceil_y_s;

		cya = clamp(ya, data.ytop[x], data.ybottom[x]);

        yb = (x - wall.start.x) * (floor_y_e - floor_y_s) / (wall.end.x - wall.start.x) + floor_y_s;

		cyb = clamp(yb, data.ytop[x], data.ybottom[x]);

		draw_floor_or_ceil(sdl->surf, sec->ceil_tex, x, data.ytop[x], cya, data.diff_ceil, player);

		draw_floor_or_ceil(sdl->surf, sec->floor_tex, x, cyb, data.ybottom[x], data.diff_floor, player);

		if (wall.type != empty_wall)
		{
			txtx = (u0 * ((wall.end.x - x) * wall.end.y) + u1 * ((x - wall.start.x) * wall.start.y)) / ((wall.end.x - x) * wall.end.y + (x - wall.start.x) * wall.start.y);
			textLine(x, cya, cyb, (struct Scaler)Scaler_Init(ya, cya, yb, 0, fabsf(sec->floor - sec->ceil) * scaleH), txtx, sdl->surf, wall.texture);
			data.ybottom[x] = cyb;
			data.ytop[x] = cya;
		}
		else
		{
			n_cya = clamp((x - wall.start.x) * (n_ceil_y_e - n_ceil_y_s) / (wall.end.x-wall.start.x) + n_ceil_y_s,
			data.ytop[x], data.ybottom[x]);
			n_cyb = clamp((x - wall.start.x) * (n_floor_y_e - n_floor_y_s) / (wall.end.x-wall.start.x) + 
			n_floor_y_s, data.ytop[x], data.ybottom[x]);
			
			data.ytop[x] = n_cya;
			data.ybottom[x] = n_cyb;
		}
		x++;
	}
	if (wall.type == empty_wall)
	{
		if (wall.sectors[0]->sector != player.curr_sector->sector && wall.sectors[0]->sector != sec->sector)
			draw_sectors(wall.sectors[0], player, sdl, &data);
		else if (wall.sectors[1]->sector != player.curr_sector->sector && wall.sectors[1]->sector != sec->sector)
			draw_sectors(wall.sectors[1], player, sdl, &data);
	}
}

void			*run_thread(void *param)
{
	t_super_data	*super;
	super = (t_super_data*)param;
	draw_world(super->sec, super->wall, super->player, super->sdl, super->data);
	return (NULL);
}

void			draw_sectors(t_sector *sec, t_player player, t_sdl *sdl, t_draw_data *data)
{
	int			i;
	int			d;
	int			p;
	int			wall;
	pthread_t	thread[THREADS];
	t_super_data super[THREADS];

	d = 0;
	i = 0;
	p = 0;
	data->diff_ceil = sec->ceil - player.height;
	data->diff_floor = sec->floor - player.height;
	/*
	while (d < THREADS && i < sec->n_walls)
	{
		if(sec->wall[i]->type != empty_wall)
		{
			super[d].data = *data;
			super[d].player = player;
			super[d].sdl = sdl;
			super[d].sec = sec;
			super[d].wall = *sec->wall[i];
			pthread_create(&thread[d], NULL, run_thread, &super[d]);
			d++;
		}
		i++;
	}*/
	while (i < sec->n_walls)
	{
		if(sec->wall[i]->type != empty_wall)
			draw_world(sec, *sec->wall[i], player, sdl, *data);
		i++;
	}
/*	d = 0;
	while (d < THREADS)
	{
		pthread_join(thread[d], NULL);
		d++;
	}*/
	while (p < MAX_PORTALS && sec->portals[p] >= 0)
	{
		draw_world(sec, *sec->wall[sec->portals[p]], player, sdl, *data);
		p++;
	}
}

void				run_with_buff(t_player player, t_sdl *sdl, unsigned int win_x)
{
	unsigned 		x; 
	int 			ytop[win_x];
	int				ybottom[win_x];
	t_draw_data		draw_data;

	x = 0;
    while(x < win_x)
	{
		ybottom[x] = sdl->win_size.y - 1;
		ytop[x] = 0;
		x++;
	}
	draw_data.start = 0;
	draw_data.end = win_x;
	draw_data.ytop = &ytop[0];
	draw_data.ybottom = &ybottom[0];
	draw_sectors(player.curr_sector, player, sdl, &draw_data);
}

void			move_player(t_player *player, float sin_angle, float cos_angle)
{
	int			i;

	i = 0;
	while (i < player->curr_sector->n_walls)
	{
		if(IntersectBox(player->pos.x, player->pos.y, player->pos.x + cos_angle, player->pos.y + sin_angle,
			player->curr_sector->wall[i]->start.x, player->curr_sector->wall[i]->start.y,
			player->curr_sector->wall[i]->end.x, player->curr_sector->wall[i]->end.y)
        && PointSide( player->pos.x + cos_angle, player->pos.y + sin_angle,player->curr_sector->wall[i]->start.x, player->curr_sector->wall[i]->start.y,
			player->curr_sector->wall[i]->end.x, player->curr_sector->wall[i]->end.y) < 0)
        {
			if (player->curr_sector->wall[i]->type == fieled_wall)
				return;
			if (player->curr_sector->wall[i]->sectors[0] &&
				player->curr_sector->sector != player->curr_sector->wall[i]->sectors[0]->sector) 
				player->curr_sector = player->curr_sector->wall[i]->sectors[0];
			else if (player->curr_sector->wall[i]->sectors[1] &&
				player->curr_sector->sector != player->curr_sector->wall[i]->sectors[1]->sector) 
				player->curr_sector = player->curr_sector->wall[i]->sectors[1];
			player->height = EyeHeight + player->curr_sector->floor;
			break;
        }
		i++;
	}
	player->pos = (t_vector){player->pos.x + cos_angle, player->pos.y + sin_angle};
	sort_by_nearest(player->curr_sector->wall, *player, player->curr_sector->portals, MAX_PORTALS);
}

int 				is_it_wall(t_vector pos, t_wall wall)
{
	t_point			n;
	int 			p;

	n = (t_point){wall.start.y - wall.end.y, wall.end.x - wall.start.x};
	p = (n.x * (pos.x - wall.start.x) + n.y * (pos.y - wall.start.y)) / sqrtf(n.x * n.x + n.y * n.y);
	return (p);
}

int					hook_event(t_player *player)
{
	SDL_Event		e;

	while(SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			return (0);
		else if (e.type == SDL_KEYDOWN)
		{
			if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP)
				move_player(player, player->sin_angl, player->cos_angl);
			if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN)
				move_player(player, -player->sin_angl, -player->cos_angl);
			if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT)
			{
				if (e.key.keysym.sym == SDLK_LEFT)
					player->angle -= 0.1f;	
				else
					player->angle += 0.1f;
				player->cos_angl = cos(player->angle);
				player->sin_angl = sin(player->angle);
			}
			if (e.key.keysym.sym == 'p')
			{
				scaleH++;
			}
			if (e.key.keysym.sym == 'm')
				scaleH--;
			if (e.key.keysym.sym == SDLK_a)
				move_player(player, -player->cos_angl, player->sin_angl);
			if (e.key.keysym.sym == SDLK_d)
				move_player(player, player->cos_angl, -player->sin_angl);
			if (e.key.keysym.sym == SDLK_ESCAPE)
				return (0);
		}
	}
	return (1);
}

void                game_loop(t_sdl *sdl, t_player player, t_sector *sectors)
{
    int                run;
    SDL_Texture     *tex;
    TTF_Font         *font;
    t_timer            timer;
    char            str[100];
    SDL_Texture        *text;
    SDL_Rect        fps_area;
	float 			max;
    
	max = 0;
    font = TTF_OpenFont("font.ttf", 100);
    fps_area = (SDL_Rect){20, 20, 150, 55};
    
    player.cos_angl = cos(player.angle);
    player.sin_angl = sin(player.angle);
    run = 1;
    sdl->frame_id = 0;
    timer = init_timer();
    start_timer(&timer);
    while(run)
    {
        SDL_SetRenderDrawColor(sdl->ren, 0, 0, 0, 255);
        SDL_RenderClear(sdl->ren);
        SDL_FillRect(sdl->surf, NULL, 0x00);
        run_with_buff(player, sdl, sdl->win_size.x);
        tex = SDL_CreateTextureFromSurface(sdl->ren, sdl->surf);
        sdl_render(sdl->ren, tex, NULL, NULL);
        
        SDL_DestroyTexture(tex);
        
        if((sdl->fps = (float)sdl->frame_id / (get_ticks(timer) / 1000.f)) > 2000000)
            sdl->fps = 0;
		max = sdl->fps > max ? sdl->fps : max;
        sdl->frame_id++;
        sprintf(str, "fps:  %f", sdl->fps);
        //    printf("got fps = %f\n", sdl->fps);
        
       text = make_black_text_using_ttf_font(sdl->ren, font, str);
        
        SDL_RenderCopy(sdl->ren, text, NULL, &fps_area);
        
        SDL_DestroyTexture(text);
        
        SDL_RenderPresent(sdl->ren);
        
        run = hook_event(&player);
    }
	
	printf("\n\n\t\t---- MAX FPS  %f ----\n\n", max);
    
    TTF_CloseFont(font);
    
    SDL_DestroyTexture(tex);
}


int				main(int argc, char **argv)
{
	t_sector	*sectors;
	t_player	player;
	t_sdl		*sdl;

	if (argc > 1)
		sectors = read_map(argv[1]);
	if (!sectors)
		exit(1);
	sdl = new_t_sdl(W, H, "test_sectors");
	init_sdl(sdl);
	SDL_ShowCursor(SDL_DISABLE);
	printf("tan:%f\n2f:%f\n", sdl->win_size.y / tan(350), .2f * sdl->win_size.y);
	player = (t_player){};
	player.pos = (t_vector){3, 3, 0};
	player.half_win_size = (t_point) {sdl->win_size.x / 2, sdl->win_size.y / 2};
//	player.yaw = -0.3;
	// screenHeight / tan(horizontalfov) good value tan(400) 
	player.hfov = sdl->win_size.x / tan(400) /*0.73f * sdl->win_size.y*/; 
	player.vfov = sdl->win_size.y * (1.0 * .2f);
//	player.vfov	= .2f * sdl->win_size.y /*.2f * sdl->win_size.y*/;
//	player.hfov = sdl->win_size.y * (1.0 * 0.73f * sdl->win_size.y / sdl->win_size.x);
	// screenWidth / tan(verticalfov) good value tan(350)


	player.height = EyeHeight;
	player.curr_sector = sectors;


	game_loop(sdl, player, sectors);

	list_sectors(sectors);
	
	delete_sectors(sectors);
	free_t_sdl(&sdl);
//	system("leaks -q test");
}
