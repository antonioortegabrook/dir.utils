/**
	dir.ls object by Antonio Ortega Brook	- June 2017
 
	Lists the contents of a directory (outputs full paths)
 
	los paths son absolutos y en estilo nativo; para convertirlos, ver los objetos standard.
*/

#include "ext.h"
#include "ext_obex.h"


/** Object struct
 */
typedef struct _dir_ls
{
	t_object        x_obj;
	
	// attributes
	char		showhidden;
	t_symbol	*kind;
	t_symbol	*type;
	
	long		kind_c, type_c;

	void		*ls_out;
        
} t_dir_ls;

enum custom_fileinfo_flags {
	
	CUSTOM_PATH_FILE
	
} e_custom_fileinfo_flags;

/** Function prototypes
 */
void *dir_ls_new(t_symbol *s, long argc, t_atom *argv);
void dir_ls_free(t_dir_ls *x);
void dir_ls_assist(t_dir_ls *x, void *b, long m, long a, char *s);

void dir_ls_anything(t_dir_ls *x, t_symbol *s, long argc, t_atom *argv);

int match_attr_showhidden(t_dir_ls *x, char *filename);
int match_attr_kind(t_dir_ls *x, t_fileinfo *file);
int match_attr_type(t_dir_ls *x, t_fileinfo *file);

/** Attr getters and setters
 */
t_max_err dir_ls_kind_get(t_dir_ls *x, void *attr, long *argc, t_atom **argv);
t_max_err dir_ls_kind_set(t_dir_ls *x, void *attr, long argc, t_atom *argv);

t_max_err dir_ls_type_get(t_dir_ls *x, void *attr, long *argc, t_atom **argv);
t_max_err dir_ls_type_set(t_dir_ls *x, void *attr, long argc, t_atom *argv);


/** Global class pointer variable
 */
void *dir_ls_class;


void ext_main(void *r)
{
	t_class *c;

	c = class_new("dir.ls", (method)dir_ls_new, (method)dir_ls_free, (long)sizeof(t_dir_ls),
                      0L /* leave NULL!! */, A_GIMME, 0);

        class_addmethod(c, (method)dir_ls_anything,             "anything",   A_GIMME,  0);
        
	class_addmethod(c, (method)dir_ls_assist,               "assist",     A_CANT, 0);
	
	
	// attr
	CLASS_ATTR_CHAR	(c,	"showhidden",	0,	t_dir_ls, showhidden);
//	CLASS_ATTR_SYM	(c,	"kind",		0,	t_dir_ls, kind);
	CLASS_ATTR_SYM_VARSIZE(c, "kind",	0,	t_dir_ls, kind, kind_c, 512);
	CLASS_ATTR_SYM_VARSIZE(c, "type",	0,	t_dir_ls, type, type_c, 512);
	
	// override default accessors
	CLASS_ATTR_ACCESSORS(c, "kind",		(method)dir_ls_kind_get,	(method)dir_ls_kind_set);
	CLASS_ATTR_ACCESSORS(c, "type",		(method)dir_ls_type_get,	(method)dir_ls_type_set);
	
	/* style */  /* label */
	CLASS_ATTR_STYLE_LABEL(c, "showhidden", 0, "onoff", "Show hidden");
	
	
	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	dir_ls_class = c;
}


void dir_ls_assist(t_dir_ls *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { // inlet
		sprintf(s, "path (symbol)");
	}
	else {	// outlet
		sprintf(s, "directory contents (dump, symbol)");
	}
}

void dir_ls_free(t_dir_ls *x)
{
	;
}


void *dir_ls_new(t_symbol *s, long argc, t_atom *argv)
{
        t_dir_ls *x;
        x = (t_dir_ls *)object_alloc(dir_ls_class);
	
	x->ls_out = listout(x);
	
	// init params
	x->showhidden = false;
	
	x->kind = NULL;
	x->type = NULL;
	
	x->kind_c = 0;
	x->type_c = 0;
	
	attr_args_process(x, argc, argv);
        
	return (x);
}


t_max_err dir_ls_kind_get(t_dir_ls *x, void *attr, long *argc, t_atom **argv)
{
	char alloc;
	
	if (atom_alloc_array(x->kind_c, argc, argv, &alloc))
		return MAX_ERR_GENERIC;

	if (!alloc) {
		post("alloc failed");
		return MAX_ERR_GENERIC;
	}

	for (t_int i = 0; i < x->kind_c; i++)
		atom_setsym(*argv + i, x->kind + i);

	return MAX_ERR_NONE;
}

t_max_err dir_ls_kind_set(t_dir_ls *x, void *attr, long argc, t_atom *argv)
{
	// allocate (or reallocate) x->kind
	if (x->kind)
		x->kind = (t_symbol *)sysmem_resizeptr(x->kind, argc * sizeof(t_symbol));
	else
		x->kind = (t_symbol *)sysmem_newptr(argc * sizeof(t_symbol));
	
	// put values into array
	for (int i = 0; i < argc; i++)
		x->kind[i] = *atom_getsym(argv + i);
	
	// update count
	x->kind_c = argc;

	return MAX_ERR_NONE;
}

t_max_err dir_ls_type_get(t_dir_ls *x, void *attr, long *argc, t_atom **argv)
{
	char alloc;
	
	if (atom_alloc_array(x->type_c, argc, argv, &alloc))
		return MAX_ERR_GENERIC;
	
	if (!alloc) {
		post("alloc failed");
		return MAX_ERR_GENERIC;
	}
	
	for (t_int i = 0; i < x->type_c; i++)
		atom_setsym(*argv + i, x->type + i);
	
	
	return MAX_ERR_NONE;
}

t_max_err dir_ls_type_set(t_dir_ls *x, void *attr, long argc, t_atom *argv)
{
	// allocate (or reallocate) x->type
	if (x->type)
		x->type = (t_symbol *)sysmem_resizeptr(x->type, argc * sizeof(t_symbol));
	else
		x->type = (t_symbol *)sysmem_newptr(argc * sizeof(t_symbol));
	
	// put values into array
	for (int i = 0; i < argc; i++)
		x->type[i] = *atom_getsym(argv + i);
	
	// update count
	x->type_c = argc;
	
	return MAX_ERR_NONE;
}


void dir_ls_anything(t_dir_ls *x, t_symbol *s, long argc, t_atom *argv)
{
	char *path_name;
	short path;
	char dirname[MAX_FILENAME_CHARS];

	void *folder_state;

	t_fourcc *file_type = NULL;
	char next_filename[MAX_FILENAME_CHARS];
	short descend = 0;
	
	char posix_filepath[MAX_PATH_CHARS];
	char native_filepath[MAX_PATH_CHARS];
	
	t_fileinfo file;
	
	t_atom output_filepath;
	
	if (!s) {
		object_error((t_object *)x, "Not a symbol");
		return;
	}
	
	
	path_name = s->s_name;
	
	// open dir
	if (!path_frompathname(path_name, &path, dirname)) {
		
		folder_state = path_openfolder(path);
		
		if (!(int *)folder_state) {
			
			object_error((t_object *)x, "Directory cannot be accessed");
			return;
		}
	} else {
		// OS-specific error code
		object_error((t_object *)x, "Can't conform dirname");
		return;
	}

	// iterate through directory
	while (path_foldernextfile(folder_state, file_type, next_filename, descend)) {
		
		path_fileinfo(next_filename, path, &file);
		
		path_toabsolutesystempath(path, next_filename, posix_filepath);
		
		// conform path
		path_nameconform(posix_filepath, native_filepath, PATH_STYLE_NATIVE, PATH_TYPE_ABSOLUTE);
		
		// put into atom
		atom_setsym(&output_filepath, gensym(native_filepath));

		// check attr
		if (match_attr_showhidden(x, next_filename)	&&
		    match_attr_kind(x, &file)			&&
		    match_attr_type(x, &file)) {
			
			// send data out
			outlet_anything (x->ls_out, gensym(native_filepath), 0, &output_filepath);
		}
	}
		
	// close directory
	path_closefolder(folder_state);
	
}


int match_attr_showhidden(t_dir_ls *x, char *filename)
{
	if (x->showhidden)
		return true;
	else
		return strncmp(".", filename, 1);
}


int match_attr_kind(t_dir_ls *x, t_fileinfo *file)
{
	if (!x->kind_c || !strcmp(x->kind->s_name, ""))
		return true;
	
	for (long i = 0; i < x->kind_c; i++) {
		
		if (!strcmp("file", x->kind[i].s_name) && file->flags == CUSTOM_PATH_FILE)
			return true;
		
		else if (!strcmp("alias", x->kind[i].s_name) && file->flags == PATH_FILEINFO_ALIAS)
			return true;
		
		else if (!strcmp("folder", x->kind[i].s_name) && file->flags == PATH_FILEINFO_FOLDER)
			return true;
		
		else if (!strcmp("package", x->kind[i].s_name) && file->flags == PATH_FILEINFO_PACKAGE)
			return true;

	}
	
	return false;
}


int match_attr_type(t_dir_ls *x, t_fileinfo *file)
{
	t_fourcc type_param;
	
	if (!x->type_c || !strcmp(x->type->s_name, ""))
		return true;
	
	for (long i = 0; i < x->type_c; i++) {
		
		type_param = *(t_fourcc *)x->type[i].s_name;
		STR_TO_FOURCC(type_param);
		
		if (file->type == type_param)
			return true;
	}

	return false;
}



