#include <stdio.h>

#include <libant_py_ant.h>
#include <libant_grid.h>

/* GENERAL METHODS */

PyObject * position_as_py_long(ant_t * ant, int index)
{
	
	return PyLong_FromLongLong(ant->position[index]);
	
}

//TODO general position checker

PyObject * langtons_ant_default_directive_wrapper(PyObject * module, PyObject * args)
{
	
	PyObject * arg_ant, * grid;

	if (PyArg_UnpackTuple(args, "langtons_ant_directive", 2, 2, &arg_ant, &grid))
	{

		if (!py_ant_check(arg_ant))
		{

			PyErr_SetString(PyExc_TypeError, "First argument should be of type Ant.");
			return NULL;

		}
		if (!py_grid_check(grid))
		{
			
			PyErr_SetString(PyExc_TypeError, "Second argument should be of type Grid.");

		}

		py_ant * ant = (py_ant *)arg_ant;

		Py_INCREF(ant);
		Py_INCREF(grid);

		if (ant->ant->orientation < 0 || ant->ant->orientation > 3)
		{

			PyErr_SetString(PyExc_IndexError, "Langton's ant has only four valid orientations. Orientation field should be valued between 0 and 3.");
			Py_DECREF(ant);
			Py_DECREF(grid);
			return NULL;

		}

		langtons_ant_default_directive(ant->ant, ((py_grid *)grid)->grid);

		Py_DECREF(ant);
		Py_DECREF(grid);

		Py_RETURN_NONE;

	}

	#ifdef LIBANT_DEBUG
	puts(ERROR("Argument parsing failed."));
	#endif

	if (!PyErr_Occurred()) PyErr_SetString(PyExc_Exception, "Arguments could not be parsed."); //TODO this needs to be specified

	return NULL;
	
}

PyMethodDef langtons_ant_directive = {"langtons_ant_directive", (PyCFunction) langtons_ant_default_directive_wrapper, METH_VARARGS, "Default directive for langton's ant." };

PyObject * langtons_ant_directive_func = NULL;

/* END GENERALS */

/* GETTERS */
PyTupleObject * ant_get_position(py_ant * self, void * closure)
{

	#ifdef LIBANT_DEBUG
	puts(DEBUG("Getting Ant position..."));
	#endif
	size_t size = self->ant->tuple_size;
	PyObject * position_tuple = PyTuple_New(size);
	#ifdef LIBANT_DEBUG
	printf(DEBUGLN("Size of tuple: %zu"), size);
	#endif
	for (int i = 0; i < size; i++)
	{
			
		PyTuple_SetItem(position_tuple,
						i,
						position_as_py_long(self->ant, i));
		
	}
	Py_INCREF(position_tuple);
	return (PyTupleObject *) position_tuple;

}

PyLongObject * ant_get_orientation(py_ant * self, void * closure)
{

	#ifdef LIBANT_DEBUG
	puts(DEBUG("Getting Ant orientation..."));
	#endif

	PyObject * orientation = PyLong_FromUnsignedLong(self->ant->orientation);
	Py_INCREF(orientation);
	return (PyLongObject *) orientation;

}

PyObject * ant_get_directive(py_ant * self, void * closure)
{

	Py_INCREF(self->py_directive);
	return self->py_directive;
	if(self->ant->directive == NULL)
	{

		Py_INCREF(Py_None);
		return Py_None;

	}	
	
}

/* END GETTERS */

/* SETTERS */

int ant_set_position(py_ant * self, PyTupleObject * value, void * closure)
{

	#ifdef LIBANT_DEBUG
	puts(DEBUG("Setting Ant position..."));
	#endif
	
	size_t stored_tuple_size = self->ant->tuple_size;
	if (value == NULL)
	{
		
		//zero-fill the tuple and maintain its size
		#ifdef LIBANT_DEBUG
		puts(DEBUG("Attempting to zero internal position array..."));
		#endif
		zero_ant_position(self->ant);
		return 0;
		
	}
	else if(!PyTuple_Check(value))
	{
		
		//Invalid type - not a tuple
		PyErr_SetString(PyExc_TypeError, "Position value should be a tuple.");
		return -1;
		
	}
	
	size_t incoming_tuple_size = PyTuple_GET_SIZE(value);
	#ifndef _WIN32
	PyObject * tuple_values[incoming_tuple_size];
	#else
	//TODO free
	PyObject ** tuple_values = malloc(sizeof(PyObject *) * incoming_tuple_size);
	#endif
	
	for (int i = 0; i < incoming_tuple_size; i++)
	{

		//Retreive value from tuple
		PyObject * tuple_member = PyTuple_GET_ITEM(value, (Py_ssize_t) i); 
		//Assert that tuple member is a python integer
		if (!PyLong_Check(tuple_member))
		{

			//Invalid type - not an integer
			PyErr_SetString(PyExc_TypeError, "Tuple value should be an integer.");
			return -1;

		}

		tuple_values[i] = tuple_member;

	}

	if (incoming_tuple_size != stored_tuple_size)
	{
		
		#ifdef LIBANT_DEBUG
		printf(DEBUGLN("Resizing internal position tuple to size %zu..."), incoming_tuple_size);
		#endif
		
		self->ant = resize_ant_position(self->ant, incoming_tuple_size);
		
	}

	for (int i = 0; i < incoming_tuple_size; i++)
	{
		
		//Evaluate long long int value of tuple member
		long long int int_value = PyLong_AsLongLong(tuple_values[i]);
		self->ant->position[i] = int_value;

	}

	return 0;

}

int ant_set_orientation(py_ant * self, PyObject * value, void * closure)
{

	if (!PyLong_Check(value))
	{

		//Invalid type - not an integer
		PyErr_SetString(PyExc_TypeError, "Value should be an unsigned long.");
		return -1;

	}


	unsigned long long_value = PyLong_AsUnsignedLong(value);

	if (long_value == -1 || long_value > 4294967295) //TODO this bound is really weird and not acting right, also using same value is overflow on windows
	{

		#ifdef LIBANT_DEBUG
		puts(DEBUG("Error in getting unsigned long from orientation value."));
		#endif
		
		PyErr_SetString(PyExc_ValueError, "Invalid value for orientation. Value should be unsigned and less than or equal to 4294967295."); // ULONG_MAX aka " xstr(UINT_MAX) ".");
		return -1;

	}

	#ifdef LIBANT_DEBUG
	printf(DEBUG("Setting Ant position with unsigned long value of %ld...")"\n", long_value);
	#endif

	self->ant->orientation = long_value;


	return 0;

}

int ant_set_directive(py_ant * self, PyObject * value, void * closure)
{

	Py_INCREF(self);
	
	Py_DECREF(self->py_directive);
	if (value == NULL || value == Py_None)
	{

		#ifdef LIBANT_DEBUG
		puts(DEBUG("Resetting directive references..."));
		#endif

		self->ant->directive = NULL;
		self->py_directive = Py_None;
		Py_INCREF(Py_None);
		return 0;

	}
	else if (value == langtons_ant_directive_func)
	{

		#ifdef LIBANT_DEBUG
		puts(DEBUG("Detected default directive for langton's ant algorithm. Propogating to backend."));
		#endif

		//TODO set backend directive to langton's default C version
		self->ant->directive = (ant_directivefn)langtons_ant_default_directive;

	}
	// If other default directives are defined, conditions go here
	else
	{

		//TODO self->ant->directive = somefunctiontocallfrontend;

	}

	self->py_directive = value;
	Py_DECREF(self);

	return 0;

}


/* END SETTERS */

PyObject * ant_new(PyTypeObject * type, PyObject * args, PyObject * kwargs)
{

    py_ant * self = (py_ant *) type->tp_alloc(type, 0);
    if (self != NULL)
    {

		#ifdef LIBANT_DEBUG
        puts(DEBUG("Got a new Ant!"));
		#endif

    }

    return (PyObject *) self; //TODO is NULL when None is passed to constructor

}

int ant_init(py_ant * self, PyObject * args, PyObject * kwargs)
{

	PyObject * arg = NULL;

	if (!PyArg_UnpackTuple(args, "Ant.__init__", 0, 1, &arg)) return -1;
	if (arg != NULL && !py_grid_check(arg))
	{

		PyErr_SetString(PyExc_TypeError, "Expected a Grid for single argument Ant constructor.");
		return -1;

	}

	// TODO add to scan list
	self->ant = create_ant(2);
	self->py_directive = (PyObject *) Py_None;
	Py_INCREF(Py_None);
	
    return 0;

}

void ant_dealloc(py_ant * self)
{

    Py_TYPE(self)->tp_free((PyObject *) self);

}

#ifndef _WIN32
static
#endif
PyGetSetDef ant_getsetters[] = {

	{"position", (getter) ant_get_position, (setter) ant_set_position, "ant position", NULL},
	{"orientation", (getter) ant_get_orientation, (setter) ant_set_orientation, "ant orientation", NULL},  
	{"directive", (getter) ant_get_directive, (setter) ant_set_directive, "directive function called every pass by the grid", NULL}, 
	{NULL}

};

PyTypeObject py_ant_type = {
	
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "libant.Ant",
        .tp_doc = "An ant to move on the grid.",
        .tp_basicsize = sizeof(py_ant),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
		.tp_getset = ant_getsetters,
        .tp_new = ant_new,
        .tp_init = (initproc) ant_init,
        .tp_dealloc = (destructor) ant_dealloc,
		
};

