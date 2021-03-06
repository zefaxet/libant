#include <libant_grid.h>
#include <Python.h>

#include <libant_py_ant.h>

static PyObject * py_grid_add_ant(PyObject * self, PyObject * arg)
{

	LOG(puts(TRACE("Inserting ant named %s into grid named %s."))); //TODO printf interpolation once debug names are implemented fully

	if (!py_ant_check(arg))
	{

		PyErr_SetString(PyExc_TypeError, "Expected Ant instance for argument.");
		return NULL;

	}

	grid_add_ant(((py_grid*)self)->grid, ((py_ant*)arg)->ant);

	Py_RETURN_NONE;

}

static PyObject * py_grid_get(PyObject * self, PyObject * args)
{

	LOG(puts(TRACE("Beginning retreival of grid value...")));

	PyObject * x_or_coord_tuple;
	PyObject * y = NULL;

	if (PyArg_UnpackTuple(args, "Grid.get", 1, 2, &x_or_coord_tuple, &y))
	{

		if (y == NULL)
		{

			// Other arg should be a tuple. Proceed with that assumption
			if (!PyTuple_Check(x_or_coord_tuple) || (PyTuple_Size(x_or_coord_tuple) != 2))
			{

				PyErr_SetString(PyExc_TypeError, "Single argument should be a 2-tuple.");
				return NULL;

			}

			y = PyTuple_GetItem(x_or_coord_tuple, 1);
			x_or_coord_tuple = PyTuple_GetItem(x_or_coord_tuple, 0);

		}
		
		// In any case, both variables should be PyObject * by now
		if (!PyLong_Check(x_or_coord_tuple) || !PyLong_Check(y))
		{

			PyErr_SetString(PyExc_TypeError, "Coordinate components should be integers.");
			return NULL;

		}

		long long int x_coord = PyLong_AsLongLong(x_or_coord_tuple);
		long long int y_coord = PyLong_AsLongLong(y);

		if ((x_coord == -1 || y_coord == -1) && PyErr_Occurred()) return NULL;

		PyObject * value = grid_get_value(((py_grid *)self)->grid, x_coord, y_coord);
		if (value == NULL)
		{

			LOG(puts(TRACE("No value found in grid.")));
			Py_RETURN_NONE;

		}
		else
		{

			LOG(puts(TRACE("Value found.")));
			Py_INCREF(value);
			return value;

		}

	}

	LOG(puts(ERROR("Argument parsing failed.")));

	if (!PyErr_Occurred()) PyErr_SetString(PyExc_Exception, "Arguments could not be parsed."); //TODO this needs to be specified

	return NULL;

}

static PyObject * py_grid_insert(PyObject * self, PyObject * args) //TODO could return true/false
{

	long long int x, y;
	PyObject * value;

	if(PyArg_ParseTuple(args, "LLO:Grid.insert", &x, &y, &value))
	{

		Py_INCREF(value);
		ant_grid_t * grid = ((py_grid *)self)->grid;
		grid->insert(grid, x, y, value, NULL);
		//TODO removed records need to be DECREF'd

		LOG(printf(TRACE("Insert to (%lld, %lld) successfull.\n"), x, y));

	}
	else return NULL;

	Py_RETURN_NONE;

}

static PyObject * py_grid_tick(PyObject * self)
{

	LOG(puts(TRACE("Performing state tick on py_grid...")));
	grid_tick(((py_grid *)self)->grid);
	Py_RETURN_NONE;

}

static PyObject * grid_new(PyTypeObject * type, PyObject * args, PyObject * kwargs)
{

	py_grid * self = (py_grid *) type->tp_alloc(type, 0);
	IF_DEBUG( if (self != NULL)
	{

		puts(DEBUG("Got a new Grid!"));

	})
	return (PyObject *) self;

}

static int grid_init(py_grid * self, PyObject * args, PyObject * kwargs)
{

	Py_INCREF(Py_None);
	self->grid = Py_None; //TODO Currently causes segfaults
	return 0;

}

static void grid_dealloc(py_grid * self)
{

	Py_TYPE(self)->tp_free((PyObject *) self);

}

static PyGetSetDef grid_getsetters[] = {

	{NULL}

};

static PyMethodDef grid_methods[] = {

	{"add_ant", (PyCFunction) py_grid_add_ant, METH_O, "Add an ant instance to the grid's internal scan list such that it's directive is executed with every tick of the tracking grid."},
	{"get", (PyCFunction) py_grid_get, METH_VARARGS, "Retreive the value stored in the cell at the given location. Accepts a single 2-tuple of ints or 2 ints."},
	{"insert", (PyCFunction) py_grid_insert, METH_VARARGS, "Insert the given value at the specified location."},
	{"tick", (PyCFunction) py_grid_tick, METH_NOARGS, "Updates the state of the world based on the rules of each cell in the grid's scan list."},
	{NULL}

};

PyTypeObject py_grid_type = {

	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "libant.Grid",
	.tp_doc = "A grid to represent the world in which automata simulations occur.",
	.tp_basicsize = sizeof(py_grid),
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_getset = grid_getsetters,
	.tp_methods = grid_methods,
	.tp_new = grid_new,
	.tp_init = (initproc) grid_init,
	.tp_dealloc = (destructor) grid_dealloc,

};
