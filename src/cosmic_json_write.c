#include "cosmic/cosmic_json.h"

/**
 * Writer functions
 */

ssize_t cosmic_json_write_value(const cosmic_json_t *, cosmic_io_t *,
                                const char *, size_t);

ssize_t cosmic_json_write_newline(cosmic_io_t *io, const char *indent) {
  if (!indent) {
    return 0;
  }

  return cosmic_io_write(io, "\n", 1);
}

ssize_t cosmic_json_write_indent(cosmic_io_t *io, const char *indent,
                                 int idepth) {
  int i;
  size_t ilen;
  if (!indent) {
    return 0;
  }

  ilen = strlen(indent);
  for (i = 0; i < idepth; i++) {
    if (cosmic_io_write(io, indent, ilen) <= 0) {
      return -1;
    }
  }

  return idepth * ilen;
}

ssize_t cosmic_json_write_comma(cosmic_io_t *io, cosmic_iterator_t *it) {
  if (cosmic_iterator_has_next(it)) {
    return cosmic_io_write(io, ",", 1);
  }
  return 0;
}

ssize_t cosmic_json_write_colon(cosmic_io_t *io, const char *indent) {
  if (indent) {
    return cosmic_io_write(io, ": ", 2);
  }
  return cosmic_io_write(io, ":", 1);
}

ssize_t cosmic_json_write_null(cosmic_io_t *io) {
  return cosmic_io_write(io, "null", 4);
}

ssize_t cosmic_json_write_number(const cosmic_json_t *j, cosmic_io_t *io) {
  char buf[COSMIC_NUMBER_BUFFER_LEN + 1];
  int i;
  /* Check write length */
  if ((i = snprintf(buf, COSMIC_NUMBER_BUFFER_LEN, "%.4f",
                    cosmic_json_get_number(j))) >= COSMIC_NUMBER_BUFFER_LEN) {
    return -1;
  }

  return cosmic_io_write(io, buf, i);
}

ssize_t cosmic_json_write_string(const char *cp, cosmic_io_t *io) {
  ssize_t i = 0, j;

  if ((j = cosmic_io_write(io, "\"", 1)) <= 0) {
    return -1;
  }

  i += j;

  while (*cp) {
    if (*cp == '\\' || *cp == '"') {
      if ((j = cosmic_io_write(io, "\\", 1)) <= 0) {
        return -1;
      }

      i += j;
    }

    if ((j = cosmic_io_write(io, cp++, 1)) <= 0) {
      return -1;
    }

    i += j;
  }

  if ((j = cosmic_io_write(io, "\"", 1)) <= 0) {
    return -1;
  }

  return i + j;
}

ssize_t cosmic_json_write_json_string(const cosmic_json_t *str,
                                      cosmic_io_t *io) {
  return cosmic_json_write_string(cosmic_json_get_string(str), io);
}

ssize_t cosmic_json_write_bool(const cosmic_json_t *j, cosmic_io_t *io) {
  const char *b = cosmic_json_get_bool(j) ? "true" : "false";
  return cosmic_io_write(io, b, strlen(b));
}

ssize_t cosmic_json_write_object(const cosmic_json_t *obj, cosmic_io_t *io,
                                 const char *indent, size_t idepth) {
  cosmic_pair_t p;
  cosmic_iterator_t *it = NULL;
  ssize_t i = 0, j;

  if (cosmic_json_size(obj) == 0) {
    return cosmic_io_write(io, "{}", 2);
  }

  if ((j = cosmic_io_write(io, "{", 1)) <= 0) {
    return -1;
  }

  i += j;

  if ((j = cosmic_json_write_newline(io, indent)) < 0) {
    return -1;
  }

  i += j;

  it = cosmic_json_iterator(obj);
  while (1) {
    if (cosmic_iterator_next_pair(it, &p) != 0) {
      cosmic_iterator_close(it);
      break;
    }

    if ((j = cosmic_json_write_indent(io, indent, idepth + 1)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_string(p.k.vp, io)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_colon(io, indent)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_value(p.v.vp, io, indent, idepth + 1)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_comma(io, it)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_newline(io, indent)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;
  }

  if ((j = cosmic_json_write_indent(io, indent, idepth)) < 0) {
    return -1;
  }

  i += j;

  if ((j = cosmic_io_write(io, "}", 1)) <= 0) {
    return -1;
  }

  return i + j;
}

ssize_t cosmic_json_write_list(const cosmic_json_t *list, cosmic_io_t *io,
                               const char *indent, size_t idepth) {
  cosmic_any_t o;
  cosmic_iterator_t *it = NULL;
  ssize_t i = 0, j;

  if (cosmic_json_size(list) == 0) {
    return cosmic_io_write(io, "[]", 2);
  }

  if ((j = cosmic_io_write(io, "[", 1)) <= 0) {
    return -1;
  }

  i += j;

  if ((j = cosmic_json_write_newline(io, indent)) < 0) {
    return -1;
  }

  i += j;

  it = cosmic_json_iterator(list);
  while (1) {
    if (cosmic_iterator_next(it, &o) != 0) {
      cosmic_iterator_close(it);
      break;
    }

    if ((j = cosmic_json_write_indent(io, indent, idepth + 1)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_value(o.vp, io, indent, idepth + 1)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_comma(io, it)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_newline(io, indent)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;
  }

  if ((j = cosmic_json_write_indent(io, indent, idepth)) < 0) {
    return -1;
  }

  i += j;

  if ((j = cosmic_io_write(io, "]", 1)) <= 0) {
    return -1;
  }

  return i + j;
}

ssize_t cosmic_json_write_value(const cosmic_json_t *j, cosmic_io_t *io,
                                const char *indent, size_t idepth) {
  switch (cosmic_json_get_type(j)) {
  case COSMIC_NULL:
    return cosmic_json_write_null(io);
  case COSMIC_NUMBER:
    return cosmic_json_write_number(j, io);
  case COSMIC_STRING:
    return cosmic_json_write_json_string(j, io);
  case COSMIC_BOOL:
    return cosmic_json_write_bool(j, io);
  case COSMIC_OBJECT:
    return cosmic_json_write_object(j, io, indent, idepth);
  case COSMIC_LIST:
    return cosmic_json_write_list(j, io, indent, idepth);
  }

  return -1;
}

ssize_t cosmic_json_write_stream(const cosmic_json_t *j, cosmic_io_t *io,
                                 const char *indent) {
  return cosmic_json_write_value(j, io, indent, 0);
}

ssize_t cosmic_json_write_buffer(const cosmic_json_t *j, char *buf, size_t len,
                                 const char *indent) {
  cosmic_io_t *io = cosmic_io_mem_new(buf, len);
  ssize_t i = cosmic_json_write_stream(j, io, indent);
  cosmic_io_mem_free(io);
  return i;
}