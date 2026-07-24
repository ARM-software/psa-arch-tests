/** @file
 * Copyright (c) 2026, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val.h"
#include "val_log.h"
#include <stddef.h>

enum {
    LOG_NUMBER_BUFFER_SIZE = 32
};

typedef enum {
    LENGTH_DEFAULT,
    LENGTH_CHAR,
    LENGTH_SHORT,
    LENGTH_LONG,
    LENGTH_LONGLONG,
    LENGTH_SIZE
} log_length_t;

typedef struct {
    unsigned int width;
    bool left_align;
    bool zero_pad;
    bool alternate;
    bool uppercase;
    bool show_plus;
    bool leading_space;
} log_format_t;

size_t log_buffer_offset;
char log_buffer[LOG_BUFFER_SIZE];

/**
 *   @brief    - Pushes one byte into the circular log buffer and UART output
 *   @param    - ch : Character to emit
 *   @return   - void
**/
static void log_store_char(char ch)
{
    log_buffer[log_buffer_offset] = ch;
    log_buffer_offset = (log_buffer_offset + 1U) % LOG_BUFFER_SIZE;
    pal_uart_putc(ch);
}

/**
 *   @brief    - Writes one logical character and expands newline to CRLF
 *   @param    - ch         : Character to write
 *             - written    : Running output length
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - void
**/
static void log_write_char(char ch, size_t *written, bool *line_start)
{
    if (ch == '\n')
    {
        log_store_char('\r');
        log_store_char('\n');
        *written += 2U;
        *line_start = true;
        return;
    }

    log_store_char(ch);
    (*written)++;
    *line_start = false;
}

/**
 *   @brief    - Emits the same character multiple times
 *   @param    - ch         : Character to repeat
 *             - count      : Number of repetitions
 *             - written    : Running output length
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - void
**/
static void log_write_repeat(char ch, unsigned int count, size_t *written,
                 bool *line_start)
{
    while (count-- != 0U)
        log_write_char(ch, written, line_start);
}

/**
 *   @brief    - Measures the printable length of a string, including NULL fallback
 *   @param    - text : Input string
 *   @return   - Length of the string representation
**/
static size_t log_string_length(const char *text)
{
    size_t length = 0;

    if (text == NULL)
        return 6U;

    while (text[length] != '\0')
        length++;

    return length;
}

/**
 *   @brief    - Writes a bounded character span to the log sink
 *   @param    - text       : Source string
 *             - length     : Number of bytes to emit
 *             - written    : Running output length
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - void
**/
static void log_write_span(const char *text, size_t length, size_t *written,
               bool *line_start)
{
    size_t index;
    const char *safe_text = (text == NULL) ? "(null)" : text;

    for (index = 0; index < length; ++index)
        log_write_char(safe_text[index], written, line_start);
}

/**
 *   @brief    - Writes a full string to the log sink
 *   @param    - text       : Source string
 *             - written    : Running output length
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - void
**/
static void log_write_text(const char *text, size_t *written, bool *line_start)
{
    log_write_span(text, log_string_length(text), written, line_start);
}

/**
 *   @brief    - Returns the line prefix associated with a verbosity level
 *   @param    - verbosity : Requested print level
 *   @return   - Prefix string for the log line
**/
static const char *log_prefix_for_level(print_verbosity_t verbosity)
{
    switch (verbosity)
    {
    case INFO:
        return "\t\tINFO: ";
    case DBG:
        return "\t\tDBG: ";
    case TEST:
        return "\t";
    case WARN:
        return "\t\tWARN: ";
    case ERROR:
        return "\t\tERROR: ";
    case ALWAYS:
    default:
        return "";
    }
}

/**
 *   @brief    - Parses printf flag characters from the format string
 *   @param    - fmt    : Current format cursor
 *             - format : Parsed flag state
 *   @return   - Pointer to the next unparsed format character
**/
static const char *parse_flags(const char *fmt, log_format_t *format)
{
    bool parsing = true;

    while (parsing)
    {
        switch (*fmt)
        {
        case '-':
            format->left_align = true;
            fmt++;
            break;
        case '0':
            format->zero_pad = true;
            fmt++;
            break;
        case '#':
            format->alternate = true;
            fmt++;
            break;
        case '+':
            format->show_plus = true;
            fmt++;
            break;
        case ' ':
            format->leading_space = true;
            fmt++;
            break;
        default:
            parsing = false;
            break;
        }
    }

    return fmt;
}

/**
 *   @brief    - Parses the minimum field width portion of a format specifier
 *   @param    - fmt    : Current format cursor
 *             - args   : Variadic argument list
 *             - format : Parsed width state
 *   @return   - Pointer to the next unparsed format character
**/
static const char *parse_width(const char *fmt, va_list *args, log_format_t *format)
{
    if (*fmt == '*')
    {
        int width = va_arg(*args, int);

        if (width < 0)
        {
            format->left_align = true;
            width = -width;
        }
        format->width = (unsigned int)width;
        return fmt + 1;
    }

    while ((*fmt >= '0') && (*fmt <= '9'))
    {
        format->width = (format->width * 10U) + (unsigned int)(*fmt - '0');
        fmt++;
    }

    return fmt;
}

/**
 *   @brief    - Parses the integer length modifier in a format specifier
 *   @param    - fmt    : Current format cursor
 *             - length : Parsed length selection
 *   @return   - Pointer to the next unparsed format character
**/
static const char *parse_length(const char *fmt, log_length_t *length)
{
    if (*fmt == 'h')
    {
        fmt++;
        if (*fmt == 'h')
        {
            *length = LENGTH_CHAR;
            return fmt + 1;
        }
        *length = LENGTH_SHORT;
        return fmt;
    }

    if (*fmt == 'l')
    {
        fmt++;
        if (*fmt == 'l')
        {
            *length = LENGTH_LONGLONG;
            return fmt + 1;
        }
        *length = LENGTH_LONG;
        return fmt;
    }

    if ((*fmt == 'z') || (*fmt == 'j') || (*fmt == 't'))
    {
        *length = LENGTH_SIZE;
        return fmt + 1;
    }

    *length = LENGTH_DEFAULT;
    return fmt;
}

/**
 *   @brief    - Reads an unsigned integer argument using the parsed length
 *   @param    - args   : Variadic argument list
 *             - length : Parsed length selection
 *   @return   - Unsigned value widened to 64 bits
**/
static uint64_t read_unsigned_value(va_list *args, log_length_t length)
{
    switch (length)
    {
    case LENGTH_CHAR:
        return (uint8_t)va_arg(*args, unsigned int);
    case LENGTH_SHORT:
        return (uint16_t)va_arg(*args, unsigned int);
    case LENGTH_LONG:
        return va_arg(*args, unsigned long);
    case LENGTH_LONGLONG:
        return va_arg(*args, unsigned long long);
    case LENGTH_SIZE:
        return (uint64_t)va_arg(*args, size_t);
    case LENGTH_DEFAULT:
    default:
        return va_arg(*args, unsigned int);
    }
}

/**
 *   @brief    - Reads a signed integer argument using the parsed length
 *   @param    - args   : Variadic argument list
 *             - length : Parsed length selection
 *   @return   - Signed value widened to 64 bits
**/
static int64_t read_signed_value(va_list *args, log_length_t length)
{
    switch (length)
    {
    case LENGTH_CHAR:
        return (int8_t)va_arg(*args, int);
    case LENGTH_SHORT:
        return (int16_t)va_arg(*args, int);
    case LENGTH_LONG:
        return va_arg(*args, long);
    case LENGTH_LONGLONG:
        return va_arg(*args, long long);
    case LENGTH_SIZE:
        return (int64_t)va_arg(*args, ptrdiff_t);
    case LENGTH_DEFAULT:
    default:
        return va_arg(*args, int);
    }
}

/**
 *   @brief    - Converts an unsigned integer to reversed digits in the requested base
 *   @param    - buffer    : Destination digit buffer
 *             - value     : Value to convert
 *             - base      : Numeric base
 *             - uppercase : Select uppercase hex digits when true
 *   @return   - Number of digits written into buffer
**/
static size_t format_unsigned(char *buffer, uint64_t value, unsigned int base,
                  bool uppercase)
{
    static const char digits_lower[] = "0123456789abcdef";
    static const char digits_upper[] = "0123456789ABCDEF";
    const char *digits = uppercase ? digits_upper : digits_lower;
    size_t length = 0;

    do
    {
        buffer[length++] = digits[value % base];
        value /= base;
    } while ((value != 0U) && (length < LOG_NUMBER_BUFFER_SIZE));

    return length;
}

/**
 *   @brief    - Writes a reversed digit buffer in forward order
 *   @param    - buffer     : Reversed digit storage
 *             - length     : Number of digits in buffer
 *             - written    : Running output length
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - void
**/
static void log_write_reversed(const char *buffer, size_t length, size_t *written,
                   bool *line_start)
{
    while (length-- != 0U)
        log_write_char(buffer[length], written, line_start);
}

/**
 *   @brief    - Writes a string with optional field-width padding
 *   @param    - text       : Source string
 *             - format     : Parsed format options
 *             - written    : Running output length
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - void
**/
static void log_write_padded_string(const char *text, log_format_t *format,
                    size_t *written, bool *line_start)
{
    size_t length = log_string_length(text);
    unsigned int padding = (format->width > length) ?
        (format->width - (unsigned int)length) : 0U;

    if (!format->left_align)
        log_write_repeat(' ', padding, written, line_start);

    log_write_text(text, written, line_start);

    if (format->left_align)
        log_write_repeat(' ', padding, written, line_start);
}

/**
 *   @brief    - Writes a formatted integer with sign, prefix, and width handling
 *   @param    - magnitude  : Absolute numeric value
 *             - negative   : Indicates whether to print a minus sign
 *             - base       : Numeric base
 *             - format     : Parsed format options
 *             - prefix     : Optional numeric prefix
 *             - written    : Running output length
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - void
**/
static void log_write_number(uint64_t magnitude, bool negative, unsigned int base,
                 log_format_t *format, const char *prefix,
                 size_t *written, bool *line_start)
{
    char digits[LOG_NUMBER_BUFFER_SIZE];
    size_t digit_count = format_unsigned(digits, magnitude, base, format->uppercase);
    char sign = '\0';
    size_t prefix_length = log_string_length(prefix);
    unsigned int content_length;
    unsigned int padding;

    if (negative)
        sign = '-';
    else if (format->show_plus)
        sign = '+';
    else if (format->leading_space)
        sign = ' ';

    content_length = (unsigned int)digit_count + (unsigned int)prefix_length;
    if (sign != '\0')
        content_length++;

    padding = (format->width > content_length) ? (format->width - content_length) : 0U;

    if (!format->left_align && !format->zero_pad)
        log_write_repeat(' ', padding, written, line_start);

    if (sign != '\0')
        log_write_char(sign, written, line_start);

    if (prefix_length != 0U)
        log_write_text(prefix, written, line_start);

    if (!format->left_align && format->zero_pad)
        log_write_repeat('0', padding, written, line_start);

    log_write_reversed(digits, digit_count, written, line_start);

    if (format->left_align)
        log_write_repeat(' ', padding, written, line_start);
}

/**
 *   @brief    - Expands a printf-style format string into the log sink
 *   @param    - fmt        : Format string
 *             - args       : Variadic argument list
 *             - line_start : Tracks whether the next write starts a line
 *   @return   - Number of characters emitted
**/
static size_t log_vprintf(const char *fmt, va_list *args, bool *line_start)
{
    size_t written = 0;

    while (*fmt != '\0')
    {
        log_format_t format = {0};
        log_length_t length = LENGTH_DEFAULT;

        if (*fmt != '%')
        {
            log_write_char(*fmt++, &written, line_start);
            continue;
        }

        fmt = parse_flags(fmt + 1, &format);
        fmt = parse_width(fmt, args, &format);
        fmt = parse_length(fmt, &length);

        switch (*fmt)
        {
        case '\0':
            return written;
        case '%':
            log_write_char('%', &written, line_start);
            break;
        case 'c': {
            char character[2];

            character[0] = (char)va_arg(*args, int);
            character[1] = '\0';
            log_write_padded_string(character, &format, &written, line_start);
            break;
        }
        case 's':
            log_write_padded_string(va_arg(*args, char *), &format, &written,
                        line_start);
            break;
        case 'd':
        case 'i':
        {
            int64_t value = read_signed_value(args, length);
            bool negative = (value < 0);
            uint64_t magnitude = negative ?
                ((uint64_t)(-(value + 1)) + 1U) : (uint64_t)value;

            log_write_number(magnitude, negative, 10U, &format, "", &written,
                     line_start);
            break;
        }
        case 'u':
            log_write_number(read_unsigned_value(args, length), false, 10U,
                     &format, "", &written, line_start);
            break;
        case 'x':
        case 'X':
            format.uppercase = (*fmt == 'X');
            log_write_number(read_unsigned_value(args, length), false, 16U,
                     &format, format.alternate ? (format.uppercase ? "0X" : "0x") : "",
                     &written, line_start);
            break;
        case 'p':
            format.zero_pad = true;
            format.width = (unsigned int)((sizeof(void *) * 2U) + 2U);
            log_write_number((uintptr_t)va_arg(*args, void *), false, 16U,
                     &format, "0x", &written, line_start);
            break;
        default:
            log_write_char('%', &written, line_start);
            log_write_char(*fmt, &written, line_start);
            break;
        }

        fmt++;
    }

    return written;
}

/**
 *   @brief    - Prints a formatted message through the VAL logging backend
 *   @param    - verbosity : Requested print level
 *             - msg       : Format string
 *             - ...       : Format arguments
 *   @return   - Number of emitted characters
**/
uint32_t val_printf(print_verbosity_t verbosity, const char *msg, ...)
{
    static bool line_start = true;
    size_t written = 0;
    va_list args;

    if ((msg == NULL) || (verbosity < VERBOSITY))
        return 0;

    va_start(args, msg);

    if (line_start)
        log_write_text(log_prefix_for_level(verbosity), &written, &line_start);

    written += log_vprintf(msg, &args, &line_start);

    va_end(args);
    return (uint32_t)written;
}
