#include "sector_worker.h"

t_item				*new_item()
{
	t_item			*i;

	if(!(i = (t_item*)ft_memalloc(sizeof(t_item))))
		return (NULL);
	*i = (t_item){};
	return (i);
}

t_item				*create_new_item(int x, int y)
{
	t_item			*i;

	if (!(i = new_item()))
		return (NULL);
	i->pos.x = x;
	i->pos.y = y;
	return (i);
}

void 				add_next_item(t_item **head, t_item *new)
{
	t_item			*main;
	unsigned		id;

	if (!*head || !new)
		return ;
	id = 1;
	main = *head;
	while (main->next)
	{
		main = main->next;
		id++;
	}
	new->id = id;
	main->next = new;
}

void				list_items(t_item *items)
{
	t_item			*it;

	if (!items)
		return ;
	it = items;
	while (it)
	{
		ft_putstr(it->state == 0 ? "Items # " : "Enemies # ");
		ft_putnbr(it->id);
		ft_putstr("\npos x = ");
		ft_putnbr(it->pos.x);
		ft_putstr("; y = ");
		ft_putnbr(it->pos.y);
		ft_putstr(". State ");
		ft_putstr(it->state > 1 ? "shooting" : "waiting");
		ft_putstr("; Id texture: ");
		ft_putnbr(it->id_text[0]);
		write(1, "\n", 1);
		it = it->next;
	}
}

void 				delete_item(t_item **item)
{
	if (!*item)
		return ;
	ft_memdel((void**)item);
}

void				delete_items_list(t_item *items)
{
	t_item 			*next;

	if (!items)
		return ;
	next = items;
	while(items)
	{
		next = items;
		items = next->next;
		delete_item(&next);
	}
}

void 				delete_item_by_id(t_item *items, unsigned id)
{	
	t_item			*all;
	t_item			*tmp;

	if (!items)
		return ;
	all = items;
	if (all->id == id)
		delete_item(&all);
	tmp = all->next;
	while (tmp)
	{
		if (tmp->id == id)
		{
			all->next = tmp->next;
			delete_item(&tmp);
			break ;
		}
		all = tmp;
		tmp = tmp->next;
	}
}