#include "cosmic/cosmic_json.h"

ssize_t cosmic_json_read_value(cosmic_io_t *io, cosmic_json_t **, char, char *);

/**
 * Read into c until expect, digit, or invalid character
 */
ssize_t cosmic_json_seek_until(cosmic_io_t *io, char *c, const char *expect,
                               int digit) {
  size_t i = 0;
  while (cosmic_io_read(io, c, 1) > 0) {
    if (strchr(expect, *c)) {
      return i;
    }

/* Negative will be classed as a digit */
    if (digit && (isdigit(*c) || *c == '-')) {
      return i;
    }

    if (!strchr(COSMIC_JSON_WHITESPACE, *c)) {
      return -1;
    }

    i++;
  }

  return -1;
}

/**
 * Read string into dest
 * Account for escape \\ and quote logic
 * Return number of bytes read into dest
 * Assumed dest is len + 1 for nullt
 */
ssize_t cosmic_json_read_string(cosmic_io_t *io, char *dest, size_t len) {
  char c, escape = 0;
  /**
   * i is current dest index
   * j is counter (escape str len can vary)
   */
  ssize_t n = 0;
  size_t i = 0, j = 0;
  while (i < len && (n = cosmic_io_read(io, &c, 1)) > 0) {
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

    /* Floating escape */
    if (escape) {
      return -1;
    }

    if (dest) {
      *(dest + i) = c;
    }

    i++;
  }

  if (n <= 0) {
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
int cosmic_json_read_and_compare(cosmic_io_t *io, const char *cmp) {
  char c;
  ssize_t n = 0;
  while (*cmp && (n = cosmic_io_read(io, &c, 1)) > 0) {
    if (c != *cmp) {
      return -1;
    }

    cmp++;
  }

  if (n <= 0) {
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

ssize_t cosmic_json_read_null(cosmic_io_t *io, cosmic_json_t **j) {
  const char *cmp = "ull";
  if (cosmic_json_read_and_compare(io, cmp) < 0) {
    return -1;
  }

  *j = cosmic_json_new_null();
  return 3;
}

ssize_t cosmic_json_read_number(cosmic_io_t *io, cosmic_json_t **j, char h,
                                char *overflow) {
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
      return -1;
    }

    /**
     *  Only happens once
     * We know this is invalid json (not number)
     */
    if (h == '-' && n == 0) {
      return -1;
    }

    if (!isdigit(h) && h != '.') {
      break;
    }

    *(buf + i++) = h;
  } while (i < COSMIC_NUMBER_BUFFER_LEN && (n = cosmic_io_read(io, &h, 1)) > 0);

  if (n <= 0) {
    return -1;
  }

  *(buf + i) = 0;

  d = strtod(buf, &rcheck);
  /* If we didn't read the entire string */
  if (*rcheck) {
    return -1;
  }

  *j = cosmic_json_new_number(d);

  *overflow = h;

  return i;
}

/**
 * Handles stack buffer allocation and rval check before creating json_string
 */
ssize_t cosmic_json_read_json_string(cosmic_io_t *io, cosmic_json_t **j) {
  char buf[COSMIC_STRING_MAX_LEN + 1];
  ssize_t i = cosmic_json_read_string(io, buf, COSMIC_STRING_MAX_LEN);
  if (i < 0) {
    return -1;
  }

  *j = cosmic_json_new_string(buf);
  return i;
}

/**
 * String cmp with either "true" or "false"
 */
ssize_t cosmic_json_read_bool(cosmic_io_t *io, cosmic_json_t **j, char hint) {
  const char *cmp = NULL;

  /* Hint is first char in comparison string */
  if (hint == 't') {
    cmp = "rue";
  } else {
    cmp = "alse";
  }

  if (cosmic_json_read_and_compare(io, cmp) < 0) {
    return -1;
  }

  if (hint == 't') {
    *j = cosmic_json_new_bool(1L);
  } else {
    *j = cosmic_json_new_bool(0L);
  }

  return strlen(cmp);
}

ssize_t cosmic_json_read_object(cosmic_io_t *io, cosmic_json_t **obj) {
  ssize_t j = 0;
  size_t i = 0;
  char kbuf[COSMIC_STRING_MAX_LEN + 1], buf, overflow;
  cosmic_json_t *tmp_j = cosmic_json_new_object(), *tmp_value = NULL;

  while (1) {
    overflow = 0;
    j = cosmic_json_seek_until(io, &buf, "\"}", 0);
    if (j < 0) {
      break;
    }

    i += j + 1;

    /* EOO token */
    if (buf == '}') {
      break;
    }

    /* Read key string */
    j = cosmic_json_read_string(io, kbuf, COSMIC_STRING_MAX_LEN);
    if (j < 0) {
      break;
    }

    i += j;

    /* Search for kv sep token */
    j = cosmic_json_seek_until(io, &buf, ":", 0);
    if (j < 0) {
      break;
    }

    i += j + 1;

    /* Seek to some json value */
    j = cosmic_json_seek_until(io, &buf, "\"{[tfn-", 1);
    if (j < 0) {
      break;
    }

    i += j + 1;

    /* Read value attempt */
    j = cosmic_json_read_value(io, &tmp_value, buf, &overflow);
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
      j = cosmic_json_seek_until(io, &buf, ",}", 0);
      if (j < 0) {
        break;
      }

      i += j + 1;
    } else if (overflow && !strchr(",}", overflow)) {
      j = -1;
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
    return -1;
  }

  *obj = tmp_j;

  return i;
}

ssize_t cosmic_json_read_list(cosmic_io_t *io, cosmic_json_t **list) {
  ssize_t j = 0;
  size_t i = 0;
  char buf, overlfow;
  cosmic_json_t *tmp_list = cosmic_json_new_list(), *tmp_value = NULL;

  while (1) {
    overlfow = 0;
    j = cosmic_json_seek_until(io, &buf, "\"{[tfn-]", 1);
    if (j < 0) {
      break;
    }

    i += j + 1;

    if (buf == ']') {
      break;
    }

    j = cosmic_json_read_value(io, &tmp_value, buf, &overlfow);
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
      j = cosmic_json_seek_until(io, &buf, ",]", 0);
      if (j < 0) {
        break;
      }

      i += j + 1;
    } else if (overlfow && !strchr(",]", overlfow)) {
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
    return -1;
  }

  *list = tmp_list;

  return i;
}

ssize_t cosmic_json_read_value(cosmic_io_t *io, cosmic_json_t **j, char hint,
                               char *overflow) {
  enum cosmic_json_type type = cosmic_json_get_type_from_hint(hint);
  switch (type) {
  case COSMIC_NULL:
    return cosmic_json_read_null(io, j);
  case COSMIC_NUMBER:
    return cosmic_json_read_number(io, j, hint, overflow);
  case COSMIC_STRING:
    return cosmic_json_read_json_string(io, j);
  case COSMIC_BOOL:
    return cosmic_json_read_bool(io, j, hint);
  case COSMIC_OBJECT:
    return cosmic_json_read_object(io, j);
  case COSMIC_LIST:
    return cosmic_json_read_list(io, j);
  }

  return -1;
}

cosmic_json_t *cosmic_json_read_stream(cosmic_io_t *io) {
  ssize_t j;
  char buf, overflow = 0;
  cosmic_json_t *json = NULL;

  j = cosmic_json_seek_until(io, &buf, "\"{[tfn", 1);
  if (j < 0) {
    return NULL;
  }

  j = cosmic_json_read_value(io, &json, buf, &overflow);
  if (j < 0) {
    return NULL;
  }

  return json;
}

cosmic_json_t *cosmic_json_read_buffer(char *buf, size_t len) {
  cosmic_io_t *io = cosmic_io_mem_new(buf, len);
  cosmic_json_t *j = cosmic_json_read_stream(io);
  cosmic_io_mem_free(io);
  return j;
}
