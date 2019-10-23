#ifndef LIBANT_GRID_H
#define LIBANT_GRID_H

#include <stdlib.h>

#include <quadtree/quadtree.h>
#include "standards.h"
#include "cell.h"

struct ant_grid_t;

typedef struct ant_grid_t ant_grid_t;

struct ant_grid_t // eventually other data structures should be inlined castable extensions of this
{

	//TODO void * (* get)(INT x, INT y);
	//TODO void (* insert)(INT x, INT y, void * value);

	size_t scan_list_size;

	ant_cell_t ** scan_list; //TODO consider linked list

	libant_quadtree_t * tree; // TODO will need to be heavily generalized
	// TODO ^ perhaps the generalization should occur here and this member should be a value instead of a pointer

};

inline ant_cell_t * grid_get_cell(ant_grid_t * grid, INT x, INT y)
{

	return (ant_cell_t *)qt_get(grid->tree, x, y);

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

inline void grid_insert(ant_grid_t * grid, INT x, INT y, void * value)
{

	qt_insert(grid->tree, x, y, value);

}

inline void grid_scan_list_clear(ant_grid_t * grid)
{

	#ifdef LIBANT_DEBUG
	puts(TRACE("Clearing grid scan list."));
	#endif
	free(grid->scan_list);
	grid->scan_list_size = 0;
	grid->scan_list = NULL; // This will get moved by the first call to realloc

}

inline void grid_scan_list_add(ant_grid_t * grid, INT x, INT y)
{

	#ifdef LIBANT_DEBUG
	printf(TRACE("Adding cell at (%lld, %lld) to scan list.\n"), x, y);
	#endif
	ant_cell_t * cell = grid_get_cell(grid, x, y);
	grid->scan_list = realloc(grid->scan_list, sizeof(ant_cell_t *) * grid->scan_list_size); // TODO find alternative, this is unsafe
	grid->scan_list[grid->scan_list_size++] = cell;

}

ant_grid_t * new_grid(/*ant_cell_t * origin*/); // TODO bring it back but make it generic

inline void grid_tick(ant_grid_t * grid)
{

	printf("scan list size %zu\n", grid->scan_list_size);
	#ifdef LIBANT_DEBUG
	printf(TRACE("Performing state tick on ant_grid with scan list of size %zu.\n"), grid->scan_list_size);
	#endif

	for (int i = 0; i < grid->scan_list_size; i++)
	{

		#ifdef LIBANT_DEBUG
		printf(DEBUG("Updating cell at index %d with it's rule."), i);
		#endif

		ant_cell_t * cell = grid->scan_list[i];
		cell->rule(cell); //TODO need to make sure rule exists for prototype

	}

}

#endif