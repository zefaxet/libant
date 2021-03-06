#include <antalgo.h>

const int NORTH_2D[2] = {0, -1};
const int EAST_2D[2] = {1, 0};
const int SOUTH_2D[2] = {0, 1};
const int WEST_2D[2] = {-1, 0};

const int * CARDINAL_2D[4] = {NORTH_2D, EAST_2D, SOUTH_2D, WEST_2D};

void langtons_ant_default_directive(ant_t * ant, ant_grid_t * grid)
{
	
	LOG(puts(TRACE("Executing langton's ant default directive.")));


	ant_cell_t * occupied_cell = grid_get_cell(grid, ant->position[0], ant->position[1]);
	int * value = NULL;
	if (occupied_cell == NULL)
	{

		grid->insert(grid, ant->position[0], ant->position[1], (void *)1, NULL);
		int temp_value = 1;
		value = &temp_value;

	}
	else
	{

		value = (void *)&(occupied_cell->value);
		*value = !(*value);

	}

	ant->orientation = (ant->orientation + (*value ? 1 : -1)) % 4;

	INT x = ant->position[0] + CARDINAL_2D[ant->orientation][0];
	INT y = ant->position[1] + CARDINAL_2D[ant->orientation][1];
	ant->position[0] = x;
	ant->position[1] = y;
	
}

ant_t * create_langtons_ant()
{
	
	ant_t * ant = create_ant(2);
	ant->directive = (ant_directivefn)langtons_ant_default_directive;
	return ant;
	
}

// cast-compatible for cell_rulefn tupe
void langtons_ant_default_rule(ant_cell_t * cell, ant_t * ant)
{

	LOG(printf(TRACELN("Executing langton's ant default rule for cell occupied by ant at (%lld, %lld)."), ant->position[0], ant->position[1]));
	int * value = cell->value;
	*value = !(*value); //TODO this should be a rotation, langton's ant states can have limitless enumerable states

	ant->orientation = (ant->orientation + (*value ? 1 : -1)) % 4;
	ant->directive(ant);

}
