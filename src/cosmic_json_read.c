#include "cosmic/cosmic_json.h"
#include "internal/cosmic_json.h"

struct cosmic_json_read_st {
  cosmic_io_t *io;
  size_t depth; /* Function depth */
  cosmic_json_error_t *error;
};

/**
 * Increments function depth and check
 */
int cosmic_json_rw_new_func(struct cosmic_json_read_st *rw) {
  return ++rw->depth > COSMIC_MAX_DEPTH;
}

ssize_t cosmic_json_read_value(struct cosmic_json_read_st rw, cosmic_json_t **,
                               char, char *);

/**
 * Read into c until expect, digit, or invalid character
 */
ssize_t cosmic_json_seek_until(struct cosmic_json_read_st rw, char *c,
                               const char *expect, int digit) {
  size_t i = 0;
  while (cosmic_io_read(rw.io, c, 1) > 0) {
    if (strchr(expect, *c)) {
      return i;
    }

    /* Negative will be classed as a digit */
    if (digit && (isdigit(*c) || *c == '-')) {
      return i;
    }

    if (!strchr(COSMIC_JSON_WHITESPACE, *c)) {
      cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, i + 1, __func__);
      return -1;
    }

    i++;
  }

  cosmic_json_error_ctor(rw.error, COSMIC_IO_ERROR, i, __func__);
  return -1;
}

/**
 * Read string into dest
 * Account for escape \\ and quote logic
 * Return number of bytes read into dest
 * Assumed dest is len + 1 for nullt
 */
ssize_t cosmic_json_read_string(struct cosmic_json_read_st rw, char *dest,
                                size_t len) {
  char c, escape = 0;
  /**
   * i is current dest index
   * j is counter (escape str len can vary)
   */
  ssize_t n = 0;
  size_t i = 0, j = 0;
  while (i < len && (n = cosmic_io_read(rw.io, &c, 1)) > 0) {
    j++;

    /* Unescape the escape */
    if (c == '\\' && escape) {
      escape = 0;
    } else if (c == '\\') { /* Enter escape */
      escape = c;
      continue;
    }

    /* Unescaped quote signals end of string */
    if (c == '"' && !escape) {
      break;
    } else if (c == '"') { /* Escaped quote */
      escape = 0;
    }

    /* Newline */
    if (c == 'n' && escape) {
      c = '\n';
      escape = 0;
    }

    /* Floating escape */
    if (escape) {
      cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, j, __func__);
      return -1;
    }

    if (dest) {
      *(dest + i) = c;
    }

    i++;
  }

  if (n <= 0) {
    cosmic_json_error_ctor(rw.error, COSMIC_IO_ERROR, j, __func__);
    return -1;
  }

  if (dest) {
    *(dest + i) = 0;
  }

  return j;
}

/**
 * Return 0 or -1
 */
int cosmic_json_read_and_compare(struct cosmic_json_read_st rw,
                                 const char *cmp) {
  char c;
  ssize_t n = 0;
  unsigned int len = 0;
  while (*cmp && (n = cosmic_io_read(rw.io, &c, 1)) > 0) {
    len++;
    if (c != *cmp) {
      cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, len, __func__);
      return -1;
    }

    cmp++;
  }

  if (n <= 0) {
    cosmic_json_error_ctor(rw.error, COSMIC_IO_ERROR, len, __func__);
    return -1;
  }

  return 0;
}

enum cosmic_json_type cosmic_json_get_type_from_hint(char hint) {
  switch (hint) {
  case 'n':
    return COSMIC_NULL;
  case '"':
    return COSMIC_STRING;
  case 't':
  case 'f':
    return COSMIC_BOOL;
  case '{':
    return COSMIC_OBJECT;
  case '[':
    return COSMIC_LIST;
  default:
    return COSMIC_NUMBER;
  }
}

ssize_t cosmic_json_read_null(struct cosmic_json_read_st rw,
                              cosmic_json_t **j) {
  const char *cmp = "ull";
  if (cosmic_json_read_and_compare(rw, cmp) < 0) {
    return -1;
  }

  *j = cosmic_json_new_null();
  return 3;
}

ssize_t cosmic_json_read_number(struct cosmic_json_read_st rw,
                                cosmic_json_t **j, char h, char *overflow) {
  /**
   * Read left to right into buf
   * break on non-digit invalid char
   * Let next step validation call failure as
   * number sits on stack
   */

  double d;
  ssize_t n = 0;
  size_t i = 0;
  char dec = 0, buf[COSMIC_NUMBER_BUFFER_LEN + 1], *rcheck;
  do {
    /* Allow single decimal */
    if (h == '.' && !dec) {
      dec = h;
    } else if (h == '.') {
      cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, i, __func__);
      return -1;
    }

    /**
     *  Only happens once
     * We know this is invalid json (not number)
     */
    if (h == '-' && n != 0) {
      cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, i, __func__);
      return -1;
    }

    if (h != '-' && !isdigit(h) && h != '.') {
      break;
    }

    *(buf + i++) = h;
  } while (i < COSMIC_NUMBER_BUFFER_LEN &&
           (n = cosmic_io_read(rw.io, &h, 1)) > 0);

  if (n <= 0) {
    cosmic_json_error_ctor(rw.error, COSMIC_IO_ERROR, i, __func__);
    return -1;
  }

  *(buf + i) = 0;

  d = strtod(buf, &rcheck);
  /* If we didn't read the entire string */
  if (*rcheck) {
    cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, rcheck - buf,
                           __func__);
    return -1;
  }

  *j = cosmic_json_new_number(d);

  *overflow = h;

  return i;
}

/**
 * Handles stack buffer allocation and rval check before creating json_string
 */
ssize_t cosmic_json_read_json_string(struct cosmic_json_read_st rw,
                                     cosmic_json_t **j) {
  char buf[COSMIC_STRING_MAX_LEN + 1];
  ssize_t i = cosmic_json_read_string(rw, buf, COSMIC_STRING_MAX_LEN);
  if (i < 0) {
    return -1;
  }

  *j = cosmic_json_new_string(buf);
  return i;
}

/**
 * String cmp with either "true" or "false"
 */
ssize_t cosmic_json_read_bool(struct cosmic_json_read_st rw, cosmic_json_t **j,
                              char hint) {
  const char *cmp = NULL;

  /* Hint is first char in comparison string */
  if (hint == 't') {
    cmp = "rue";
  } else {
    cmp = "alse";
  }

  if (cosmic_json_read_and_compare(rw, cmp) < 0) {
    return -1;
  }

  if (hint == 't') {
    *j = cosmic_json_new_bool(1L);
  } else {
    *j = cosmic_json_new_bool(0L);
  }

  return strlen(cmp);
}

ssize_t cosmic_json_read_object(struct cosmic_json_read_st rw,
                                cosmic_json_t **obj) {
  ssize_t j = 0;
  size_t i = 0;
  char kbuf[COSMIC_STRING_MAX_LEN + 1], buf, overflow;
  cosmic_json_t *tmp_j = NULL, *tmp_value = NULL;

  if (cosmic_json_rw_new_func(&rw)) {
    cosmic_json_error_ctor(rw.error, COSMIC_MAX_DEPTH_ERROR, 0, __func__);
    return -1;
  }

  tmp_j = cosmic_json_new_object();

  while (1) {
    overflow = 0;
    j = cosmic_json_seek_until(rw, &buf, "\"}", 0);
    if (j < 0) {
      break;
    }

    i += j + 1;

    /* EOO token */
    if (buf == '}') {
      break;
    }

    /* Read key string */
    j = cosmic_json_read_string(rw, kbuf, COSMIC_STRING_MAX_LEN);
    if (j < 0) {
      break;
    }

    i += j;

    /* Search for kv sep token */
    j = cosmic_json_seek_until(rw, &buf, ":", 0);
    if (j < 0) {
      break;
    }

    i += j + 1;

    /* Seek to some json value */
    j = cosmic_json_seek_until(rw, &buf, "\"{[tfn-", 1);
    if (j < 0) {
      break;
    }

    i += j + 1;

    /* Read value attempt */
    j = cosmic_json_read_value(rw, &tmp_value, buf, &overflow);
    if (j < 0) {
      break;
    }

    i += j;

    /* Attempt to add kv to object */
    j = cosmic_json_add_kv(tmp_j, kbuf, tmp_value);
    if (j < 0) {
      /* Read value success but set fails, tmp_value has been allocated */
      cosmic_json_free(tmp_value);
      break;
    }

    /* Check for overflow as peek or seek into buf for next tokens */
    if (!overflow || (overflow && strchr(COSMIC_JSON_WHITESPACE, overflow))) {
      j = cosmic_json_seek_until(rw, &buf, ",}", 0);
      if (j < 0) {
        break;
      }

      i += j + 1;
    } else if (overflow && !strchr(",}", overflow)) {
      j = -1;
      cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, 1, __func__);
      break;
    } else if (overflow) {
      buf = overflow;
    }

    /* Object end */
    if (buf == '}') {
      break;
    }
  }

  /* All read functions will return to j */
  if (j < 0) {
    cosmic_json_free(tmp_j);
    rw.error->index += i;
    return -1;
  }

  *obj = tmp_j;

  return i;
}

ssize_t cosmic_json_read_list(struct cosmic_json_read_st rw,
                              cosmic_json_t **list) {
  ssize_t j = 0;
  size_t i = 0;
  char buf, overlfow;
  cosmic_json_t *tmp_list = NULL, *tmp_value = NULL;

  if (cosmic_json_rw_new_func(&rw)) {
    cosmic_json_error_ctor(rw.error, COSMIC_MAX_DEPTH_ERROR, 0, __func__);
    return -1;
  }

  tmp_list = cosmic_json_new_list();

  while (1) {
    overlfow = 0;
    j = cosmic_json_seek_until(rw, &buf, "\"{[tfn-]", 1);
    if (j < 0) {
      break;
    }

    i += j + 1;

    if (buf == ']') {
      break;
    }

    j = cosmic_json_read_value(rw, &tmp_value, buf, &overlfow);
    if (j < 0) {
      break;
    }

    i += j;

    j = cosmic_json_add(tmp_list, tmp_value);
    if (j < 0) {
      cosmic_json_free(tmp_value);
      break;
    }

    if (!overlfow || (overlfow && strchr(COSMIC_JSON_WHITESPACE, overlfow))) {
      j = cosmic_json_seek_until(rw, &buf, ",]", 0);
      if (j < 0) {
        break;
      }

      i += j + 1;
    } else if (overlfow && !strchr(",]", overlfow)) {
      j = -1;
      cosmic_json_error_ctor(rw.error, COSMIC_UNEXPECTED_CHAR, 1, __func__);
      break;
    } else if (overlfow) {
      buf = overlfow;
    }

    if (buf == ']') {
      break;
    }
  }

  if (j < 0) {
    cosmic_json_free(tmp_list);
    rw.error->index += i;
    return -1;
  }

  *list = tmp_list;

  return i;
}

ssize_t cosmic_json_read_value(struct cosmic_json_read_st rw, cosmic_json_t **j,
                               char hint, char *overflow) {
  enum cosmic_json_type type = cosmic_json_get_type_from_hint(hint);
  switch (type) {
  case COSMIC_NULL:
    return cosmic_json_read_null(rw, j);
  case COSMIC_NUMBER:
    return cosmic_json_read_number(rw, j, hint, overflow);
  case COSMIC_STRING:
    return cosmic_json_read_json_string(rw, j);
  case COSMIC_BOOL:
    return cosmic_json_read_bool(rw, j, hint);
  case COSMIC_OBJECT:
    return cosmic_json_read_object(rw, j);
  case COSMIC_LIST:
    return cosmic_json_read_list(rw, j);
  default:
    break;
  }

  return -1;
}

cosmic_json_t *cosmic_json_read_stream(cosmic_io_t *io) {
  ssize_t j;
  char buf, overflow = 0;
  cosmic_json_t *json = NULL;
  cosmic_json_error_t error;
  struct cosmic_json_read_st rw;

  error.error = COSMIC_NONE;
  error.func_name = NULL;
  error.index = -1;

  rw.error = &error;
  rw.io = io;
  rw.depth = 0;

  j = cosmic_json_seek_until(rw, &buf, "\"{[tfn", 1);
  if (j < 0) {
    return cosmic_json_error_new(error);
  }

  j = cosmic_json_read_value(rw, &json, buf, &overflow);
  if (j < 0) {
    return cosmic_json_error_new(error);
  }

  return json;
}

cosmic_json_t *cosmic_json_read_buffer(char *buf, size_t len) {
  cosmic_io_t *io = cosmic_io_mem_new(buf, len);
  cosmic_json_t *j = cosmic_json_read_stream(io);
  cosmic_io_mem_free(io);
  return j;
}
