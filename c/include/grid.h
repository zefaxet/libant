#ifndef LIBANT_GRID_H
#define LIBANT_GRID_H

#ifdef LIBANT_DEBUG
#include <stdio.h>
#include <antmacro.h>
#endif

#include <stdlib.h>
#include "standards.h"
#include "cell.h"
#include "ant.h"

typedef struct ant_grid_t ant_grid_t;

typedef ant_cell_t * (* grid_getfn)(ant_grid_t *, INT, INT, ...);
typedef void (* grid_insertfn)(ant_grid_t *, INT, INT, void *, cell_rulefn, ...);

struct ant_grid_t // eventually other data structures should be inlined castable extensions of this
{

	IF_DEBUG(
	char * __name;
	)

	grid_getfn get;
	grid_insertfn insert;

	size_t scan_list_size;
	ant_t ** scan_list; //TODO consider linked list

};

inline ant_cell_t * grid_get_cell(ant_grid_t * grid, INT x, INT y)
{
	
	LOG(printf(TRACE("Getting grid cell at (%lld, %lld)."), x, y));
	return grid->get(grid, x, y);

}

inline void * grid_get_value(ant_grid_t * grid, INT x, INT y) //TODO should probably be an int
{

	ant_cell_t * cell = grid_get_cell(grid, x, y);
	if (cell) return cell->value;
	else
	{

		#ifdef TREEBUG
		puts(DEBUG("Node is NULL (none was found). Returning NULL value"));
		#endif
		return NULL;

	}

}

inline void grid_scan_list_clear(ant_grid_t * grid)
{

	LOG(puts(TRACE("Clearing grid scan list.")));
	free(grid->scan_list);
	grid->scan_list_size = 0;
	grid->scan_list = NULL; // This will get moved by the first call to realloc

}

ant_grid_t new_grid(grid_getfn, grid_insertfn); // TODO bring it back but make it generic

void grid_add_ant(ant_grid_t *, ant_t *);

inline void grid_tick(ant_grid_t * grid)
{

	LOG(printf(TRACELN("Performing state tick on ant_grid with scan list of size %zu."), grid->scan_list_size));

	for (int i = 0; i < grid->scan_list_size; i++)
	{

		LOG(printf(DEBUGLN("Updating ant_t at index %d with it's directive."), i));

		ant_t * ant = grid->scan_list[i];
		ant->directive(ant, grid); //TODO need to make sure rule exists for prototype

	}

}

#endif
