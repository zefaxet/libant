#include <stdio.h>
#include "py_ant_2d_cartesian.h"
#include "antmacro.h"

/* GETTERS */

PyLongObject * ant_2d_cart_get_x(py_ant * self, void * closure)
{

	#ifdef LIBANT_DEBUG
	puts(DEBUG("Getting Ant position x value..."));
	#endif

	PyObject * x = PyLong_FromLong(self->ant->position[0]);
	Py_INCREF(x);
	return (PyLongObject *) x;

}

PyLongObject * ant_2d_cart_get_y(py_ant * self, void * closure)
{

	#ifdef LIBANT_DEBUG
	puts(DEBUG("Getting Ant position y value..."));
	#endif

	PyObject * y = PyLong_FromLong(self->ant->position[1]);
	Py_INCREF(y);
	return (PyLongObject *) y;

}

/* SETTERS */

int ant_2d_cart_set_position(py_ant * self, PyTupleObject * value, void * closure)
{

	#ifdef LIBANT_DEBUG
	puts(DEBUG("Setting 2D Cartesian Ant position..."));
	#endif
	
	if (value == NULL)
	{
		
		//zero-fill the tuple and maintain its size
		#ifdef LIBANT_DEBUG
		puts(DEBUG("Attempting to zero internal position array..."));
		#endif
		self->ant = zero_ant_position(self->ant, 2);
		return 0;
		
	}
	else if(!PyTuple_Check(value))
	{
		
		//Invalid type - not a tuple
		PyErr_SetString(PyExc_TypeError, "Position value should be a duple for a 2D Cartesian Ant.");
		return -1;
		
	}
	else if (PyTuple_GET_SIZE(value) != 2)
	{
		
		//Tuple is not correct size
		PyErr_SetString(PyExc_ValueError, "Position value must be a duple (tuple of size 2).");
		return -1;
		
	}
	
	return ant_set_position(self, value, closure);

}

int ant_2d_cart_set_x(py_ant * self, PyLongObject * value, void * closure)
{
	
	#ifdef LIBANT_DEBUG
	puts(DEBUG("Setting Ant position x value..."));
	#endif
	
	if (value == NULL)
	{
		
		#ifdef LIBANT_DEBUG
		puts(DEBUG("Setting x to zero in leu of deletion..."));
		#endif
		self->ant->position[0] = 0;
		return 0;
		
	}
	else if(!PyLong_Check(value))
	{
		
		//Invalid type - not an int
		PyErr_SetString(PyExc_TypeError, "Position values should be signed integers.");
		return -1;
		
	}
	
	self->ant->position[0] = PyLong_AsLongLong((PyObject *) value);
	
	return 0;
	
}

/* END SETTERS */

#ifndef _WIN32
static
#endif
PyGetSetDef ant_2d_cart_getsetters[] = {

	{"position", (getter) ant_get_position, (setter) ant_2d_cart_set_position, "ant position", NULL},
	{"x", (getter) ant_2d_cart_get_x, (setter) ant_2d_cart_set_x, "ant position", NULL},
	{"y", (getter) ant_2d_cart_get_y, (setter) NULL, "ant position", NULL},
	{NULL}

};

PyTypeObject py_ant_2d_cartesian_type = {
	
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "libant.Ant2DCartesian",
    .tp_doc = "2D ant",
    .tp_basicsize = sizeof(py_ant),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_getset = ant_2d_cart_getsetters,
		
};
