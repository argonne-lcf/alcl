extern int read_from_binary(unsigned char **output, size_t *size, const char *name);

extern const char *getErrorString(cl_int error);

extern void check_error(cl_int error, char const *name);

extern void exit_msg(char const *str);
