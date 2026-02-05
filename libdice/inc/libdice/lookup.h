#ifndef libdice_lookup_h
#define libdice_lookup_h

/**
 * @brief format of lookup
 * | key_length | value_length | key | value |
 * Both key and value' length are variable.
 * key is terminated by nul-character
 * key must be aligned 4-byte 
 */

#define LIBDICE_LOOKUP_SECTION_LEN 16
#define LIBDICE_LOOKUP_METADATA_LEN 1
#define LIBDICE_LOOKUP_KEY_MAX_LEN 15


#endif
