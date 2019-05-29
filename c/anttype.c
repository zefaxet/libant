#include "anttype.h"
#include "string.h"

ant_t * create_ant(size_t position_size)
{

	ant_t * ant = resize_ant_position(NULL, position_size);
	zero_ant_position(ant);
	ant->orientation = 0;
	return ant;

}

void zero_ant_position(ant_t * ant)
{
	
	memset(ant->position, 0, ant->tuple_size * sizeof(long long int));
	
}

ant_t * resize_ant_position(ant_t * ant, size_t size)
{
	
	ant = realloc(ant, sizeof(ant_t) + size * sizeof(long long int));
	ant->tuple_size = size;
	
	return ant;
	
}

ant_grid_t new_grid(ant_cell_t * origin)
{

	ant_grid_t * grid = grid = calloc(1, sizeof(ant_grid_t));
	grid->origin = origin;
	return * grid;

}

ant_cell_t new_cell_empty()
{

	ant_cell_t * cell = calloc(1, sizeof(ant_cell_t));
	return * cell;

}
