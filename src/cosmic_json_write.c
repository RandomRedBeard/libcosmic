#include "cosmic/cosmic_json.h"

struct cosmic_json_write_st {
  cosmic_io_t *io;
  const char *indent, *fprec;
  size_t idepth;
};

/**
 * Writer functions
 */

size_t cosmic_json_wr_enter(struct cosmic_json_write_st *wr) {
  return ++wr->idepth;
}

size_t cosmic_json_wr_exit(struct cosmic_json_write_st *wr) {
  return --wr->idepth;
}

ssize_t cosmic_json_write_value(const cosmic_json_t *,
                                struct cosmic_json_write_st);

ssize_t cosmic_json_write_newline(struct cosmic_json_write_st wr) {
  if (!wr.indent) {
    return 0;
  }

  return cosmic_io_write(wr.io, "\n", 1);
}

ssize_t cosmic_json_write_indent(struct cosmic_json_write_st wr) {
  size_t i, ilen;
  if (!wr.indent) {
    return 0;
  }

  ilen = strlen(wr.indent);
  for (i = 0; i < wr.idepth; i++) {
    if (cosmic_io_write(wr.io, wr.indent, ilen) <= 0) {
      return -1;
    }
  }

  return wr.idepth * ilen;
}

ssize_t cosmic_json_write_comma(cosmic_io_t *io, cosmic_iterator_t *it) {
  if (cosmic_iterator_has_next(it)) {
    return cosmic_io_write(io, ",", 1);
  }
  return 0;
}

ssize_t cosmic_json_write_colon(struct cosmic_json_write_st wr) {
  if (wr.indent) {
    return cosmic_io_write(wr.io, ": ", 2);
  }
  return cosmic_io_write(wr.io, ":", 1);
}

ssize_t cosmic_json_write_null(cosmic_io_t *io) {
  return cosmic_io_write(io, "null", 4);
}

ssize_t cosmic_json_write_number(const cosmic_json_t *j, cosmic_io_t *io,
                                 const char *fprec) {
  char buf[COSMIC_NUMBER_BUFFER_LEN + 1];
  int i;
  /* Check write length */
  if ((i = snprintf(buf, COSMIC_NUMBER_BUFFER_LEN, fprec,
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

    if (*cp == '\n') {
      if ((j = cosmic_io_write(io, "\\n", 2)) <= 0) {
        return -1;
      }

      i += j;
      cp++;
      continue;
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

ssize_t cosmic_json_write_object(const cosmic_json_t *obj,
                                 struct cosmic_json_write_st wr) {
  cosmic_pair_t p;
  cosmic_iterator_t *it = NULL;
  ssize_t i = 0, j;

  if (cosmic_json_size(obj) == 0) {
    return cosmic_io_write(wr.io, "{}", 2);
  }

  if ((j = cosmic_io_write(wr.io, "{", 1)) <= 0) {
    return -1;
  }

  cosmic_json_wr_enter(&wr);

  i += j;

  if ((j = cosmic_json_write_newline(wr)) < 0) {
    return -1;
  }

  i += j;

  it = cosmic_json_iterator(obj);
  while (1) {
    if (cosmic_iterator_next_pair(it, &p) != 0) {
      cosmic_iterator_close(it);
      break;
    }

    if ((j = cosmic_json_write_indent(wr)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_string(p.k.vp, wr.io)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_colon(wr)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_value(p.v.vp, wr)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_comma(wr.io, it)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_newline(wr)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;
  }

  cosmic_json_wr_exit(&wr);

  if ((j = cosmic_json_write_indent(wr)) < 0) {
    return -1;
  }

  i += j;

  if ((j = cosmic_io_write(wr.io, "}", 1)) <= 0) {
    return -1;
  }

  return i + j;
}

ssize_t cosmic_json_write_list(const cosmic_json_t *list,
                               struct cosmic_json_write_st wr) {
  cosmic_any_t o;
  cosmic_iterator_t *it = NULL;
  ssize_t i = 0, j;

  if (cosmic_json_size(list) == 0) {
    return cosmic_io_write(wr.io, "[]", 2);
  }

  if ((j = cosmic_io_write(wr.io, "[", 1)) <= 0) {
    return -1;
  }

  cosmic_json_wr_enter(&wr);

  i += j;

  if ((j = cosmic_json_write_newline(wr)) < 0) {
    return -1;
  }

  i += j;

  it = cosmic_json_iterator(list);
  while (1) {
    if (cosmic_iterator_next(it, &o) != 0) {
      cosmic_iterator_close(it);
      break;
    }

    if ((j = cosmic_json_write_indent(wr)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_value(o.vp, wr)) <= 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_comma(wr.io, it)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;

    if ((j = cosmic_json_write_newline(wr)) < 0) {
      cosmic_iterator_close(it);
      return -1;
    }

    i += j;
  }

  cosmic_json_wr_exit(&wr);

  if ((j = cosmic_json_write_indent(wr)) < 0) {
    return -1;
  }

  i += j;

  if ((j = cosmic_io_write(wr.io, "]", 1)) <= 0) {
    return -1;
  }

  return i + j;
}

ssize_t cosmic_json_write_value(const cosmic_json_t *j,
                                struct cosmic_json_write_st wr) {
  switch (cosmic_json_get_type(j)) {
  case COSMIC_NULL:
    return cosmic_json_write_null(wr.io);
  case COSMIC_NUMBER:
    return cosmic_json_write_number(j, wr.io, wr.fprec);
  case COSMIC_STRING:
    return cosmic_json_write_json_string(j, wr.io);
  case COSMIC_BOOL:
    return cosmic_json_write_bool(j, wr.io);
  case COSMIC_OBJECT:
    return cosmic_json_write_object(j, wr);
  case COSMIC_LIST:
    return cosmic_json_write_list(j, wr);
  default:
    break;
  }

  return -1;
}

ssize_t cosmic_json_write_stream_s(const cosmic_json_t *j, cosmic_io_t *io,
                                   const char *indent, const char *fprec) {
  struct cosmic_json_write_st wr;
  wr.io = io;
  wr.indent = indent;
  wr.idepth = 0;
  wr.fprec = fprec;
  return cosmic_json_write_value(j, wr);
}

ssize_t cosmic_json_write_buffer_s(const cosmic_json_t *j, char *buf,
                                   size_t len, const char *indent,
                                   const char *fprec) {
  cosmic_io_t *io = cosmic_io_mem_new(buf, len);
  ssize_t i = cosmic_json_write_stream_s(j, io, indent, fprec);
  cosmic_io_mem_free(io);
  return i;
}
