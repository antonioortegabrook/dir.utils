/**
	file.route object by Antonio Ortega Brook	- June 2017
 
	Receives paths and routes them to different outputs according to several criteria:
 
	@beginswith:
 
 
	if argument is only a number (or a number and a dot), use quotation marks
	Attributes are hidden from inspector because they should not be changed once te object is instantiated
 
	TODO: documentation
 
*/

#include "ext.h"
#include "ext_obex.h"

/** Object struct
 */
typedef struct _file_route
{
	t_object        x_obj;

	// keep count of attr args
	long		attr_argcount;
	
	// attributes
	t_symbol	*beginswith, *endswith;

	t_symbol	*attr_argval;
	t_int		*attr_name;
	
	// default (rightmost) outlet
	void		*default_out;

} t_file_route;

/** Attribute names
 */
enum attr_names {
	
	C_BEGINSWITH,
	C_ENDSWITH

} e_attr_names;

/** Function prototypes
 */
void *file_route_new(t_symbol *s, long argc, t_atom *argv);
void file_route_free(t_file_route *x);
void file_route_assist(t_file_route *x, void *b, long m, long a, char *s);

void file_route_list(t_file_route *x, t_symbol *s, long argc, t_atom *argv);

/** Attr getters and setters
 */
t_max_err file_route_beginswith_get(t_file_route *x, void *attr, long *argc, t_atom **argv);
t_max_err file_route_beginswith_set(t_file_route *x, void *attr, long argc, t_atom *argv);

t_max_err file_route_endswith_get(t_file_route *x, void *attr, long *argc, t_atom **argv);
t_max_err file_route_endswith_set(t_file_route *x, void *attr, long argc, t_atom *argv);

/** Other functions
 */
int begins_with(char *beggining, char *full_str);
int ends_with(char *end, char *full_str);

/** Global class pointer variable
 */
void *file_route_class;


void ext_main(void *r)
{
	t_class *c;

	c = class_new("file.route", (method)file_route_new, (method)file_route_free, (long)sizeof(t_file_route),
                      0L /* leave NULL!! */, A_GIMME, 0);

	class_addmethod(c, (method)file_route_list,		"anything",	A_GIMME,  0);

	class_addmethod(c, (method)file_route_assist,		"assist",	A_CANT, 0);
	
	// attr
	CLASS_ATTR_SYM(c, "beginswith",	0,	t_file_route, beginswith);
	CLASS_ATTR_SYM(c, "endswith",	0,	t_file_route, endswith);
	
	// override default accessors
	CLASS_ATTR_ACCESSORS(c, "beginswith", (method)file_route_beginswith_get, (method)file_route_beginswith_set);
	CLASS_ATTR_ACCESSORS(c, "endswith", (method)file_route_endswith_get, (method)file_route_endswith_set);

	// hide attribute from inspector
	CLASS_ATTR_INVISIBLE(c, "beginswith", 0);
	CLASS_ATTR_INVISIBLE(c, "beginswith", 0);


	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	file_route_class = c;
}


void file_route_assist(t_file_route *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { // inlet
		sprintf(s, "path (symbol)");
	}
	else {	// outlet
		if (a < x->attr_argcount) {
			switch (x->attr_name[a]) {
					
				case C_BEGINSWITH:
					sprintf(s, "full path if filename begins with %s", x->attr_argval[a].s_name);
					break;
					
				case C_ENDSWITH:
					sprintf(s, "full path if filename ends with %s", x->attr_argval[a].s_name);
			}
		} else {
			sprintf(s, "default");
		}

	}
}

void file_route_free(t_file_route *x)
{
	;
}


void *file_route_new(t_symbol *s, long argc, t_atom *argv)
{
	t_file_route *x = NULL;
        x = (t_file_route *)object_alloc(file_route_class);

	// default (rightmost) outlet
	x->default_out = listout(x);

	// initialize attr arg count
	x->attr_argcount = 0;

	// initialize param
	x->attr_argval	= NULL;

	// initialize comparison
	x->attr_name	= NULL;

	attr_args_process(x, argc, argv);
	
	// create aditional outlets
	for (int i = 0; i < x->attr_argcount; i++)
		outlet_append((t_object *)x, NULL, gensym("list"));


	return (x);
}


t_max_err file_route_beginswith_get(t_file_route *x, void *attr, long *argc, t_atom **argv)
{
	return MAX_ERR_NONE;
}


t_max_err file_route_beginswith_set(t_file_route *x, void *attr, long argc, t_atom *argv)
{
	// allocate (or reallocate) x->param
	if (x->attr_argval)
		x->attr_argval = (t_symbol *)sysmem_resizeptr(x->attr_argval, (x->attr_argcount + argc) * sizeof(t_symbol));
	else
		x->attr_argval = (t_symbol *)sysmem_newptr(argc * sizeof(t_symbol));

	
	// allocate (or reallocate) x->comparison
	if (x->attr_name)
		x->attr_name = (t_int *)sysmem_resizeptr(x->attr_name, (x->attr_argcount + argc) * sizeof(t_int));
	else
		x->attr_name = (t_int *)sysmem_newptr(argc * sizeof(t_int));

	
	// put param & comparison
	t_int j = 0;
	for (t_int i = x->attr_argcount; i < x->attr_argcount + argc; i++) {
		
		x->attr_argval[i] = *atom_getsym(argv + j);
		x->attr_name[i] = C_BEGINSWITH;
		
		j++;
	}
	

	// update attr arg count
	x->attr_argcount += argc;
	
	return MAX_ERR_NONE;
}


t_max_err file_route_endswith_get(t_file_route *x, void *attr, long *argc, t_atom **argv)
{
	return MAX_ERR_NONE;
}


t_max_err file_route_endswith_set(t_file_route *x, void *attr, long argc, t_atom *argv)
{
	// allocate (or reallocate) x->attr_argval
	if (x->attr_argval)
		x->attr_argval = (t_symbol *)sysmem_resizeptr(x->attr_argval, (x->attr_argcount + argc) * sizeof(t_symbol));
	else
		x->attr_argval = (t_symbol *)sysmem_newptr(argc * sizeof(t_symbol));
	
	
	// allocate (or reallocate) x->attr_name
	if (x->attr_name)
		x->attr_name = (t_int *)sysmem_resizeptr(x->attr_name, (x->attr_argcount + argc) * sizeof(t_int));
	else
		x->attr_name = (t_int *)sysmem_newptr(argc * sizeof(t_int));
	
	
	// put argval & name
	t_int j = 0;
	for (t_int i = x->attr_argcount; i < x->attr_argcount + argc; i++) {
		
		x->attr_argval[i] = *atom_getsym(argv + j);
		x->attr_name[i] = C_ENDSWITH;
		
		j++;
	}
	

	// update attr arg count
	x->attr_argcount += argc;
	
	return MAX_ERR_NONE;
}


void file_route_list(t_file_route *x, t_symbol *s, long argc, t_atom *argv)
{
        t_atom full_path;
	int match = false;
	long default_outlet = x->attr_argcount;
	
	short path;
	char filename[MAX_FILENAME_CHARS];
	t_fileinfo file;
	
	if (path_frompathname(s->s_name, &path, filename)) {
		// error
		return;
	}
	
	path_fileinfo(filename, path, &file);
	
	atom_setsym(&full_path, s);
	
	// iterate outlets
	for (t_int i = x->attr_argcount - 1; i >= 0; i--) {
		
		switch (x->attr_name[i]) {
				
			case C_BEGINSWITH:
				if (begins_with(x->attr_argval[i].s_name, filename)) {
					outlet_anything(outlet_nth((t_object *)x, i), s, 0, &full_path);
					match = true;
				}
				break;
				
			case C_ENDSWITH:
				if (ends_with(x->attr_argval[i].s_name, filename)) {
					outlet_anything(outlet_nth((t_object *)x, i), s, 0, &full_path);
					match = true;
				}
				break;
		}
	}
	
	if (!match)
		outlet_anything(outlet_nth((t_object *)x, default_outlet), s, 0, &full_path);
}



int begins_with(char *beginning, char *full_str)
{
	t_int size = strlen(beginning);
	int result;
	
	result = !strncmp(beginning, full_str, size);
	
	return result;
}



int ends_with(char *end, char *full_str)
{
	t_int size1 = strlen(end);
	t_int size2 = strlen(full_str);
	char *last_chars;
	int result;

	if (size1 > size2)
		return false;

	last_chars = &full_str[size2 - size1];

	result = !strcmp(end, last_chars);

	return result;
}
