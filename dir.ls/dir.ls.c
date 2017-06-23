/**
	Lists the contents of a directory
 
	los paths son absolutos y en estilo nativo; para convertirlos, ver los objetos standard.
*/

#include "ext.h"
#include "ext_obex.h"


/** Object struct
 */
typedef struct _dir_ls
{
	t_object        x_obj;

	void		*ls_out;
        
} t_dir_ls;

/** Function prototypes
 */
void *dir_ls_new(t_symbol *s, long argc, t_atom *argv);
void dir_ls_free(t_dir_ls *x);
void dir_ls_assist(t_dir_ls *x, void *b, long m, long a, char *s);

void dir_ls_anything(t_dir_ls *x, t_symbol *s, long argc, t_atom *argv);


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


void *dir_ls_new()
{
        t_dir_ls *x;
        x = (t_dir_ls *)object_alloc(dir_ls_class);
	
	x->ls_out = listout(x);
        
	return (x);
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
		
		path_toabsolutesystempath(path, next_filename, posix_filepath);
		
		// conform path
		path_nameconform(posix_filepath, native_filepath, PATH_STYLE_NATIVE, PATH_TYPE_ABSOLUTE);
		
		// put into atom
		atom_setsym(&output_filepath, gensym(native_filepath));

		// send data out
		outlet_anything (x->ls_out, gensym(native_filepath), 0, &output_filepath);
	}
		
	// close directory
	path_closefolder(folder_state);
	
}

