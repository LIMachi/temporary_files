#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

typedef struct	s_poly_line
{
	size_t		size;
	int			*vertices;
}				t_poly_line;

typedef union	u_vertice_data
{
	struct {
		int		vertice;
		int		texture;
		int		normal;
	};
	int			a[3];
}				t_vertice_data;

typedef struct	s_face
{
	size_t			size;
	t_vertice_data	*data;
}				t_face;

typedef union	u_double4
{
	struct {
		double		x;
		double		y;
		double		z;
		double		w;
	};
	double			a[4];
}				t_double4;

typedef struct	s_double4_array
{
	size_t		size;
	t_double4	*data;
}				t_double4_array;

typedef struct	s_face_array
{
	size_t		size;
	t_face		*data;
}				t_face_array;

typedef struct	s_poly_line_array
{
	size_t		size;
	t_poly_line	*data;
}				t_poly_line_array;

typedef struct	s_obj_file
{
	t_double4_array		vertices;
	t_double4_array		textures;
	t_double4_array		normals;
	t_double4_array		parametrics;
	t_face_array		faces;
	t_poly_line_array	lines;
}				t_obj_file;

#define A_NULL {.size = 0, .data = NULL}
#define OBJ_FILE_NULL (t_obj_file){A_NULL, A_NULL, A_NULL, A_NULL, A_NULL, A_NULL}

#define DEFAULT_VERTICE (t_double4){.a = {0.0, 0.0, 0.0, 1.0}}
#define DEFAULT_TEXTURE (t_double4){.a = {0.0, 0.0, 0.0, 0.0}}
#define DEFAULT_NORMAL  (t_double4){.a = {0.0, 0.0, 1.0, 0.0}}

char	*stratok_r(char *str, char *sep, char *saveptr[2])
{
	char	*stop;
	char	*out;

	if (saveptr == NULL || sep == NULL)
	{
		errno = EINVAL;
		return (NULL);
	}
	if (str == NULL)
		str = (*saveptr)[0];
	if ((*saveptr)[0] == NULL || (*saveptr)[0] != str)
	{
		(*saveptr)[0] = str;
		(*saveptr)[1] = str;
	}
	if (*str != '\0')
		str += strspn(str, sep);
	if (*str == '\0')
	{
		(*saveptr)[1] = str;
		return (NULL);
	}
	stop = str + strspn(str, sep)
	out = strndup(str, (size_t)stop - (size_t)str);
	(*saveptr)[1] = stop;
	if (*stop != '\0')
		++(*saveptr)[1];
	return (out);
}

void	load_vector(t_double4_array *ar, char *saveptr, t_double4 def)
{
	t_double4	*double4_ptr;
	int			i;
	char		*tok;
	char		*last_char;

	double4_ptr = &ar->data[ar->size - 1];
	i = -1;
	while (++i < 4 && (tok = strtok_r(NULL, " ", &saveptr)) != NULL)
	{
		errno = 0;
		double4_ptr->a[i] = strtod(tok, &last_char);
		if (errno == ERANGE)
			printf("Warning: overflow/underflow occured on malformated floating number\n");
		if (*last_char != '\0' && !isspace(*last_char))
			printf("Warning: discarded invalid characters after floating number: '%s'\n", last_char);
	}
	--i;
	while (++i < 4)
		double4_ptr->a[i] = def.a[i];
}

void	load_face(t_face_array *ar, char *saveptr, char *line)
{
	int		i[2];
	t_face	*face;
	char	*super_tok;
	char	*lower_tok;
	char	*last_char;

	printf("in-face\n");
	i[0] = -1;
	face = &ar->data[ar->size - 1];
	while (++i[0] < 2)
	{
		face->size = 0;
		if (i[0])
			strtok_r(line, " ", &saveptr);
		while ((super_tok = strtok_r(NULL, " ", &saveptr)) != NULL)
		{
			if (i[0])
			{
				i[1] = -1;
				lower_tok = strtok(super_tok, "/");
				while (++i[1] < 3 && lower_tok != NULL)
				{
					errno = 0;
					face->data[face->size].a[i[1]] = strtol(lower_tok, &last_char, 0);
					if (errno == ERANGE)
						printf("Warning: overflow/underflow occured on malformated index\n");
					if (*last_char != '\0' && !isspace(*last_char))
						printf("Warning: discarded invalid characters after index: '%s'\n", last_char);
					lower_tok = strtok(NULL, "/");
				}
			}
			++face->size;
		}
		if (!i[0])
			face->data = malloc(sizeof(t_vertice_data) * face->size);
	}
}

void	print_face(t_face *face)
{
	printf("face: (size: %d)\n", face->size);
	for (int i = 0; i < face->size; ++i)
		printf("\tv: %d, vt: %d, vn: %d\n", face->data[i].a[0],
			face->data[i].a[1], face->data[i].a[2]);
}

int		main(int argc, char **argv)
{
	FILE		*file;
	char		*line = NULL;
	char		*sline;
	size_t		len = 0;
	ssize_t		read;
	char		*saveptr;
	char		*tok;
	int			tok_index;
	t_obj_file	obj = OBJ_FILE_NULL;

	if (argc != 2)
	{
		printf("expected obj file has first argument\n");
		return (0);
	}
	errno = 0;
	file = fopen(argv[1], "r");
	switch (errno)
	{
		case 0: break;
		case EACCES: //permission denied
		case EFAULT: //out of address space
		case ENOMEM: //out of memory
		default:
			return (errno);
	}
	int pass = -1;
	while (++pass < 2)
	{
		while ((read = getline(&line, &len, file)) != -1)
		{
			tok_index = 0;
			if ((tok = strtok_r(line, " ", &saveptr)) == NULL || tok[0] == '#')
				continue ;
			printf("saveptr: '%s'\n", saveptr);
			//v vt vn vp f l o mtllib usemtl g s
			if (!strcmp(tok, "f") && ++obj.faces.size && pass)
			 	load_face(&obj.faces, saveptr, line);
			else if (!strcmp(tok, "v") && ++obj.vertices.size && pass)
				load_vector(&obj.vertices, saveptr, DEFAULT_VERTICE);
			else if (!strcmp(tok, "vt") && ++obj.textures.size && pass)
				load_vector(&obj.textures, saveptr, DEFAULT_TEXTURE);
			else if (!strcmp(tok, "vn") && ++obj.normals.size && pass)
				load_vector(&obj.normals, saveptr, DEFAULT_TEXTURE);
		}
		if (!pass)
		{
			obj.vertices.data = malloc(sizeof(double) * obj.vertices.size);
			obj.textures.data = malloc(sizeof(double) * obj.textures.size);
			obj.normals.data = malloc(sizeof(double) * obj.normals.size);
			obj.faces.data = malloc(sizeof(t_face) * obj.faces.size);
			obj.vertices.size = obj.textures.size = obj.normals.size = obj.faces.size = 0;
			fseek(file, 0, SEEK_SET);
		}
	}
	printf("v: %zu, vt: %zu, vn: %zu, f: %zu\n",
		obj.vertices.size, obj.textures.size,
		obj.normals.size, obj.faces.size);
	printf("first vertice: x: %f, y: %f, z: %f, w: %f\n",
		obj.vertices.data[0].x, obj.vertices.data[0].y,
		obj.vertices.data[0].z, obj.vertices.data[0].w);
	// print_face(obj.faces.data);
	free(line);
	fclose(file);
	exit(EXIT_SUCCESS);
}